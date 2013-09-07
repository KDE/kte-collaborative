/*  This file is part of kobby
    Copyright (c) 2013 Sven Brauch <svenbrauch@gmail.com>


    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kio_infinity.h"
#include "malloc.h"

#include <kdebug.h>
#include <kcomponentdata.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <klocale.h>

#include <qcoreapplication.h>
#include <kdirnotify.h>
#include <libinftext/inf-text-session.h>
#include <libinftext/inf-text-default-buffer.h>

#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/xmlconnection.h>
#include <libqinfinity/xmppconnection.h>
#include <libqinfinity/init.h>
#include <libqinfinity/qgobject.h>
#include <libqinfinity/qgsignal.h>
#include <libqinfinity/noderequest.h>
#include <libqinfinity/explorerequest.h>
#include <libqinfinity/qtio.h>

#include "common/itemfactory.h"
#include "common/noteplugin.h"
#include "common/utils.h"

using namespace KIO;
using QInfinity::QGObject;
using QInfinity::QGSignal;
using QInfinity::NodeRequest;
using QInfinity::ExploreRequest;

extern "C" {

int KDE_EXPORT kdemain( int argc, char **argv )
{
    QCoreApplication app(argc, argv);
    KComponentData componentData("infinity", "kio_infinity");

    kDebug() << "starting infinity kioslave";
    if (argc != 4) {
        kWarning() << "wrong arguments count";
        exit(-1);
    }

    ensureKdedModuleLoaded();

    QInfinity::init();

    InfinityProtocol slave(argv[2], argv[3]);
    slave.dispatchLoop();

    kDebug() << "slave exiting";
    return app.exec();
}

}

InfinityProtocol::InfinityProtocol(const QByteArray& pool_socket, const QByteArray& app_socket)
    : QObject()
    , SlaveBase("inf", pool_socket, app_socket)
    , m_notePlugin(0)
{
    kDebug() << "constructing infinity kioslave";
    connect(this, SIGNAL(requestError(GError*)), this, SLOT(slotRequestError(GError*)));
}

void InfinityProtocol::get(const KUrl& url )
{
    kDebug() << "GET " << url.url();
    if ( ! doConnect(Peer(url)) ) {
        return;
    }

    bool ok = false;
    iterForUrl(url, &ok);
    if ( ! ok ) {
        error(KIO::ERR_COULD_NOT_STAT, i18n("Could not get %1: The node does not exist.", url.url()));
        return;
    }

    mimeType("text/plain");
    // TODO: Maybe we can find a way to make the note's (current, stationary) content accessible to
    // applications which don't support infinity, too? That will need replaying the operations,
    // though, and is thus not trivial.
    data("");
    finished();
}

void InfinityProtocol::stat(const KUrl& url)
{
    kDebug() << "STAT " << url.url();
    if ( ! doConnect(Peer(url)) ) {
        return;
    }

    bool ok = false;
    QInfinity::BrowserIter iter = iterForUrl(url, &ok);
    Q_UNUSED(iter);
    if ( ! ok ) {
        error(KIO::ERR_COULD_NOT_STAT, i18n("Could not stat %1: No such file or directory.", url.url()));
        return;
    }

    UDSEntry entry;
    entry.insert(KIO::UDSEntry::UDS_MIME_TYPE, QString::fromLatin1("text/plain"));
    entry.insert(KIO::UDSEntry::UDS_NAME, iter.name());
    entry.insert(KIO::UDSEntry::UDS_SIZE, 0);
    entry.insert(KIO::UDSEntry::UDS_DISPLAY_NAME, iter.name());
    entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, iter.isDirectory() ? S_IFDIR : S_IFREG);
    entry.insert(KIO::UDSEntry::UDS_ACCESS, 0x777);
    statEntry(entry);

    finished();
}

bool InfinityProtocol::isConnectedTo(const Peer& peer)
{
    if ( m_connectedTo != peer ) {
        return false;
    }
    if ( ! m_connection || ! m_connection->xmppConnection() ) {
        return false;
    }
    if ( m_connection->xmppConnection()->status() != QInfinity::XmlConnection::Open ) {
        return false;
    }
    return true;
}

bool InfinityProtocol::doConnect(const Peer& peer)
{
    if ( isConnectedTo(peer) ) {
        return true;
    }

    QEventLoop loop;
    m_connection = QSharedPointer<Kobby::Connection>(new Kobby::Connection(peer.hostname, peer.port, QString(), this));
    m_browserModel = QSharedPointer<QInfinity::BrowserModel>(new QInfinity::BrowserModel( this ));
    m_browserModel->setItemFactory(new Kobby::ItemFactory( this ));
    QObject::connect(m_connection.data(), SIGNAL(ready(Connection*)), &loop, SLOT(quit()));
    QObject::connect(m_connection.data(), SIGNAL(error(Connection*,QString)), &loop, SLOT(quit()));
    m_connection->prepare();

    m_notePlugin = new Kobby::NotePlugin(this);
    m_browserModel->addPlugin(*m_notePlugin);

    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(connectTimeout() * 1000);
    connect(&timeout, SIGNAL(timeout()), &loop, SLOT(quit()));
    timeout.start();
    loop.exec();
    if ( ! timeout.isActive() || ! m_connection->xmppConnection() ) {
        kDebug() << "failed to look up hostname";
        error(KIO::ERR_UNKNOWN_HOST, peer.hostname);
        return false;
    }
    m_browserModel->addConnection(static_cast<QInfinity::XmlConnection*>(m_connection->xmppConnection()), "kio_root");
    m_connection->open();

    connect(browser(), SIGNAL(connectionEstablished(const QInfinity::Browser*)),
            &loop, SLOT(quit()));
    connect(browser(), SIGNAL(error(const QInfinity::Browser*,QString)),
            &loop, SLOT(quit()));
    loop.exec();
    if ( ! timeout.isActive() || browser()->connectionStatus() != INFC_BROWSER_CONNECTED ) {
        kDebug() << "failed to connect";
        error(KIO::ERR_COULD_NOT_CONNECT, QString("%1:%2").arg(peer.hostname, QString::number(peer.port)));
        return false;
    }

    m_connectedTo = peer;
    return true;
}


void InfinityProtocol::mimetype(const KUrl & url)
{
    kDebug() << "MIMETYPE" << url;
    if ( ! doConnect(Peer(url)) ) {
        return;
    }
    mimeType("text/plain");
    finished();
}

void InfinityProtocol::put(const KUrl& url, int /*permissions*/, JobFlags /*flags*/)
{
    kDebug() << "PUT" << url;
    if ( ! doConnect(Peer(url)) ) {
        return;
    }
    QByteArray buffer, initialContents;
    int size = 0, bytesRead = 1;
    while ( bytesRead != 0 ) {
        dataReq();
        bytesRead = readData(buffer);
        initialContents.append(buffer);
        size += bytesRead;
        kDebug() << "read" << bytesRead << "bytes";
    }
#ifdef ENABLE_TAB_HACK
    initialContents = initialContents.replace('\t', "    ");
#endif
    if ( size < 0 ) {
        error(KIO::ERR_INTERNAL, "Failed to read data");
        return;
    }
    QInfinity::BrowserIter iter = iterForUrl(url.upUrl());
    QInfinity::NodeRequest* req = 0;
    kDebug() << "adding note with content:" << size << "bytes";
    if ( size > 0 ) {
        // There is actually data to add to the node
        InfUser* user = INF_USER(g_object_new(
                INF_TEXT_TYPE_USER,
                "id", 1,
                "flags", INF_USER_LOCAL,
                "name", "Initial document contents",
                "status", INF_USER_INACTIVE,
                "caret-position", 0,
                static_cast<void*>(NULL)));

        InfUserTable* user_table = inf_user_table_new();
        inf_user_table_add_user(user_table, user);
        g_object_unref(user);

        InfTextDefaultBuffer* textBuffer = inf_text_default_buffer_new("UTF-8");

        InfCommunicationManager* communication_manager =
                infc_browser_get_communication_manager(INFC_BROWSER(browser()->gobject()));

        InfIo* io = INF_IO(QInfinity::QtIo::instance()->gobject());

        InfTextSession* session = inf_text_session_new_with_user_table(
                communication_manager, INF_TEXT_BUFFER(textBuffer), io,
                user_table, INF_SESSION_RUNNING, NULL, NULL);
        inf_text_buffer_insert_text(INF_TEXT_BUFFER(textBuffer), 0, initialContents,
                                    initialContents.size(), QString(initialContents).size(), user);

        req = NodeRequest::wrap(infc_browser_add_note_with_content(
                INFC_BROWSER(browser()->gobject()), iter.infBrowserIter(),
                url.fileName().toAscii().data(), m_notePlugin->infPlugin(), INF_SESSION(session), true));
        g_object_unref(session);
        g_object_unref(user_table);
        inf_session_set_user_status(INF_SESSION(session), user, INF_USER_UNAVAILABLE);
    }
    else {
        // There is no data to add, just create a new empty node
        req = browser()->addNote(iter, url.fileName().toAscii().data(), *m_notePlugin, false);
    }
    connect(req, SIGNAL(finished(NodeRequest*)), this, SIGNAL(requestSuccessful(NodeRequest*)));
    connect(req, SIGNAL(failed(GError*)), this, SIGNAL(requestError(GError*)));
    if ( waitForCompletion() ) {
        finished();
    }
}

void InfinityProtocol::del(const KUrl& url, bool /*isfile*/)
{
    kDebug() << "DELETE" << url;
    if ( ! doConnect(Peer(url)) ) {
        return;
    }
    bool itemExists = false;
    QInfinity::BrowserIter iter = iterForUrl(url, &itemExists);
    if ( ! itemExists ) {
        error(KIO::ERR_CANNOT_DELETE, i18n("Cannot delete %1: No such file or directory", url.url()));
        return;
    }
    QInfinity::NodeRequest* req = browser()->removeNode(iter);
    connect(req, SIGNAL(finished(NodeRequest*)), this, SIGNAL(requestSuccessful(NodeRequest*)));
    connect(req, SIGNAL(failed(GError*)), this, SIGNAL(requestError(GError*)));
    if ( waitForCompletion() ) {
        finished();
    }
}

void InfinityProtocol::mkdir(const KUrl& url, int /*permissions*/)
{
    kDebug() << "MKDIR" << url;
    if ( ! doConnect(Peer(url)) ) {
        return;
    }
    QInfinity::BrowserIter iter = iterForUrl(url.upUrl());
    QInfinity::NodeRequest* req = browser()->addSubdirectory(iter, url.fileName().toAscii().data());
    connect(req, SIGNAL(finished(NodeRequest*)), this, SIGNAL(requestSuccessful(NodeRequest*)));
    connect(req, SIGNAL(failed(GError*)), this, SIGNAL(requestError(GError*)));
    if ( waitForCompletion() ) {
        finished();
    }
}

void InfinityProtocol::slotRequestError(GError* error)
{
    m_lastError = QString(error->message);
}

QInfinity::BrowserIter InfinityProtocol::iterForUrl(const KUrl& url, bool* ok)
{
    KUrl clean(url);
    clean.cleanPath(KUrl::SimplifyDirSeparators);
    IterLookupHelper helper(clean.path(KUrl::AddTrailingSlash), browser());
    QEventLoop loop;
    connect(&helper, SIGNAL(done(QInfinity::BrowserIter)), &loop, SLOT(quit()));
    connect(&helper, SIGNAL(failed()), &loop, SLOT(quit()));
    helper.beginLater();
    // Using an event loop is okay in this case, because the kio slave doesn't get
    // any signals from outside.
    loop.exec();
    if ( ok ) {
        *ok = helper.success();
    }
    return helper.result();
}

void InfinityProtocol::listDir(const KUrl &url)
{
    kDebug() << "LIST DIR" << url;
    if ( ! doConnect(Peer(url)) ) {
        return;
    }

    if ( url.path().isEmpty() ) {
        KUrl newUrl(url);
        newUrl.setPath("/");
        redirection(newUrl);
        finished();
        return;
    }

    QInfinity::BrowserIter iter = iterForUrl(url);

    if ( ! iter.isExplored() ) {
        ExploreRequest* req = iter.explore();
        connect(req, SIGNAL(finished(ExploreRequest*)), this, SIGNAL(requestSuccessful(NodeRequest*)));
        connect(req, SIGNAL(failed(GError*)), this, SIGNAL(requestError(GError*)));
        if ( ! waitForCompletion() ) {
            return;
        }
    }
    bool hasChildren = iter.child();

    // If not, the directory is just empty.
    if ( hasChildren ) {
        do {
            UDSEntry entry;
            entry.insert( KIO::UDSEntry::UDS_NAME, iter.name() );
            entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, iter.isDirectory() ? S_IFDIR : S_IFREG );
            entry.insert( KIO::UDSEntry::UDS_ACCESS, 0x777 );
            listEntry(entry, false);
        } while ( iter.next() );
    }

    listEntry(UDSEntry(), true);
    finished();
}

bool InfinityProtocol::waitForCompletion()
{
    QEventLoop loop;

    // Set up the timeout connection
    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(connectTimeout() * 1000);
    connect(&timeout, SIGNAL(timeout()), &loop, SLOT(quit()));
    timeout.start();

    // Set up the connection for handling an error
    connect(this, SIGNAL(requestError(GError*)), &loop, SLOT(quit()));

    // Set up the connection for successfully completing the operation
    connect(this, SIGNAL(requestSuccessful(NodeRequest*)), &loop, SLOT(quit()));

    // Start waiting.
    loop.exec();

    if ( ! timeout.isActive() ) {
        // If the timer timed out (i.e. is not running any more), connecting failed.
        error(ERR_SERVER_TIMEOUT, i18n("Connection timed out."));
        return false;
    }

    if ( ! m_lastError.isEmpty() ) {
        // TODO is there a way to give the proper error types for e.g. "node already exists"?
        error(ERR_SLAVE_DEFINED, m_lastError);
        m_lastError.clear();
        return false;
    }
    return true;
}

QInfinity::Browser* InfinityProtocol::browser() const
{
    return m_browserModel->browsers().first();
}

