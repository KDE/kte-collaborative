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
#include <kencodingprober.h>

#include <qcoreapplication.h>
#include <qapplication.h>

#include <common/itemfactory.h>
#include <common/noteplugin.h>
#include <common/documentbuilder.h>

#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/xmlconnection.h>
#include <libqinfinity/xmppconnection.h>
#include <libqinfinity/init.h>
#include <libqinfinity/qgobject.h>
#include <libqinfinity/qgsignal.h>

#include "common/utils.h"

#define TIMEOUT_MS 10000

using namespace KIO;
using QInfinity::QGObject;
using QInfinity::QGSignal;

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

    QInfinity::init();

    InfinityProtocol slave(argv[2], argv[3]);
    slave.dispatchLoop();

    kDebug() << "slave exiting";
    return app.exec();
}

}

InfinityProtocol* InfinityProtocol::_self = 0;

InfinityProtocol::InfinityProtocol(const QByteArray& pool_socket, const QByteArray& app_socket)
    : QObject()
    , SlaveBase("inf", pool_socket, app_socket)
    , m_notePlugin(0)
{
    kDebug() << "constructing infinity kioslave";
    _self = this;
}

InfinityProtocol* InfinityProtocol::self()
{
    return _self;
}

InfinityProtocol::~InfinityProtocol()
{
    _self = 0;
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
    m_connection = QSharedPointer<Kobby::Connection>(new Kobby::Connection(peer.hostname, peer.port, this));
    m_browserModel = QSharedPointer<QInfinity::BrowserModel>(new QInfinity::BrowserModel( this ));
    m_browserModel->setItemFactory(new Kobby::ItemFactory( this ));
    QObject::connect(m_connection.data(), SIGNAL(ready(Connection*)), &loop, SLOT(quit()));
    m_connection->prepare();

    m_notePlugin = new Kobby::NotePlugin(this);
    m_browserModel->addPlugin(*m_notePlugin);

    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(TIMEOUT_MS);
    connect(&timeout, SIGNAL(timeout()), &loop, SLOT(quit()));
    timeout.start();
    loop.exec();
    if ( ! timeout.isActive() ) {
        kDebug() << "timed out looking up hostname";
        error(KIO::ERR_COULD_NOT_CONNECT, i18n("Failed to look up hostname %1: Operation timed out.", peer.hostname));
        return false;
    }
    m_browserModel->addConnection(static_cast<QInfinity::XmlConnection*>(m_connection->xmppConnection()), "kio_root");
    m_connection->open();

    while ( browser()->connectionStatus() != INFC_BROWSER_CONNECTED ) {
        QCoreApplication::processEvents();
        usleep(1000);
        if ( ! timeout.isActive() ) {
            kDebug() << "failed to connect";
            error(KIO::ERR_COULD_NOT_CONNECT, i18n("Failed to connect to host %1, port %2: Operation timed out.", peer.hostname, peer.port));
            return false;
        }
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
    QInfinity::BrowserIter iter = iterForUrl(url.upUrl());
    // TODO: this is technically wrong; we need to wait for a nodeAdded() signal
    // where the BrowserIter matches the iter found above.
    // (This actually causing problems is very unlikely though)
    connect(browser(), SIGNAL(nodeAdded(BrowserIter)), this, SIGNAL(requestSuccessful()));
    InfcNodeRequest* req = browser()->addNote(iter, url.fileName().toAscii().data(), *m_notePlugin, false);
    if ( waitForRequest(INFC_REQUEST(req)) ) {
        finished();
    }
}

void InfinityProtocol::del(const KUrl& url, bool isfile)
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
    connect(browser(), SIGNAL(nodeRemoved(BrowserIter)), this, SIGNAL(requestSuccessful()));
    InfcNodeRequest* req = browser()->removeNode(iter);
    if ( waitForRequest(INFC_REQUEST(req)) ) {
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
    // TODO: see put()
    connect(browser(), SIGNAL(nodeAdded(BrowserIter)), this, SIGNAL(requestSuccessful()));
    InfcNodeRequest* req = browser()->addSubdirectory(iter, url.fileName().toAscii().data());
    if ( waitForRequest(INFC_REQUEST(req)) ) {
        finished();
    }
}

void InfinityProtocol::requestError_cb(InfcRequest* /*request*/, GError* error, void* user_data)
{
    kDebug() << "request error:" << error->message;
    reinterpret_cast<InfinityProtocol*>(user_data)->signalError(QString(error->message));
}

void InfinityProtocol::signalError(const QString message)
{
    m_lastError = message;
    emit requestError();
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

    QInfinity::BrowserIter iter = iterForUrl(url);

    if ( ! iter.isExplored() ) {
        InfcExploreRequest* request = iter.explore();
        while ( INFC_IS_EXPLORE_REQUEST(request) && ! infc_explore_request_get_finished(INFC_EXPLORE_REQUEST(request)) ) {
            QCoreApplication::processEvents();
        }
    }
    bool hasChildren = iter.child();

    // If not, the directory is just empty.
    if ( hasChildren ) {
        do {
            UDSEntry entry;
            entry.insert( KIO::UDSEntry::UDS_URL, url.url(KUrl::AddTrailingSlash) + iter.name() );
            entry.insert( KIO::UDSEntry::UDS_NAME, iter.name() );
            entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, iter.isDirectory() ? S_IFDIR : S_IFREG );
            listEntry(entry, false);
        } while ( iter.next() );
    }

    listEntry(UDSEntry(), true);
    finished();
}

bool InfinityProtocol::waitForRequest(const InfcRequest* infcRequest)
{
    QEventLoop loop;

    // Set up the timeout connection
    QTimer timeout;
    timeout.setSingleShot(true);
    timeout.setInterval(TIMEOUT_MS);
    connect(&timeout, SIGNAL(timeout()), &loop, SLOT(quit()));
    timeout.start();

    // Set up the connections for handling an error
    QGObject request;
    request.setGobject(G_OBJECT(infcRequest));
    QGSignal connection(&request, "failed", G_CALLBACK(InfinityProtocol::requestError_cb), (void*) this);
    connect(this, SIGNAL(requestError()), &loop, SLOT(quit()));

    // Set up the connection for successfully completing the operation
    connect(this, SIGNAL(requestSuccessful()), &loop, SLOT(quit()));

    // Start waiting.
    loop.exec();

    // Disconnect the "requestSuccessful" connection, since it differs for each request
    // and is meant to be used only once
    disconnect(this, SIGNAL(requestSuccessful()));

    if ( ! m_lastError.isEmpty() ) {
        // TODO is there a way to give the proper error types for e.g. "node already exists"?
        error(ERR_SLAVE_DEFINED, m_lastError);
        m_lastError.clear();
        return false;
    }
    return true;
    // Exiting from the function will destroy the QGSignal instance and disconnect the signal.
}

QInfinity::Browser* InfinityProtocol::browser() const
{
    return m_browserModel->browsers().first();
}

