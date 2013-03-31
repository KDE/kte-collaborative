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

#include <kdebug.h>
#include <kcomponentdata.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kencodingprober.h>
#include <qcoreapplication.h>
#include <qapplication.h>

#include <kobby/editor/itemfactory.h>
#include <noteplugin.h>
#include <documentbuilder.h>
#include <remotebrowserview.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/xmlconnection.h>
#include <libqinfinity/xmppconnection.h>
#include <libqinfinity/init.h>

#include "malloc.h"
#include <KTextEditor/View>

using namespace KIO;

extern "C" {

int KDE_EXPORT kdemain( int argc, char **argv )
// int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
//     QApplication app(argc, argv);
    KComponentData componentData("infinity", "kio_infinity");

    qDebug() << "starting infinity kioslave";
    if (argc != 4) {
        qDebug() << "wrong arguments count";
        exit(-1);
    }

    QInfinity::init();

    InfinityProtocol slave(argv[2], argv[3]);
    slave.dispatchLoop();
//     slave.listDir(KUrl("inf://localhost"));

    qDebug() << "slave exiting";
    return app.exec();
}

}

InfinityProtocol* InfinityProtocol::_self = 0;

InfinityProtocol::InfinityProtocol(const QByteArray &pool_socket, const QByteArray &app_socket)
    : QObject(), SlaveBase("inf", pool_socket, app_socket)
{
    qDebug() << "constructing infinity kioslave";
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

    QString title, section;

    // tell the mimetype
    mimeType("text/plain");
    data("Hello World from kioslave!");
    finished();
}

void InfinityProtocol::stat( const KUrl& url)
{
    kDebug() << "ENTERING STAT " << url.url();
/*
    QString title, section;

    if (!parseUrl(url.path(), title, section))
    {
        error(KIO::ERR_MALFORMED_URL, url.url());
        return;
    }

    kDebug(7107) << "URL " << url.url() << " parsed to title='" << title << "' section=" << section;

    UDSEntry entry;
    entry.insert(KIO::UDSEntry::UDS_NAME, title);
    entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG);

#if 0 // not useful, is it?
    QString newUrl = "man:"+title;
    if (!section.isEmpty())
        newUrl += QString("(%1)").arg(section);
    entry.insert(KIO::UDSEntry::UDS_URL, newUrl);
#endif

    entry.insert(KIO::UDSEntry::UDS_MIME_TYPE, QString::fromLatin1("text/html"));

    statEntry(entry);*/

    finished();
}


void InfinityProtocol::mimetype(const KUrl & /*url*/)
{
    mimeType("text/plain");
    finished();
}

void InfinityProtocol::listDir(const KUrl &url)
{
    kDebug() << "LIST DIR" << url;
    kDebug() << url.host() << url.userName() << url.password() << url.path();

    m_connection = new Kobby::Connection(url.host(), 6523, this);
    m_browserModel = new QInfinity::BrowserModel( this );
    m_browserModel->setItemFactory( new Kobby::ItemFactory( this ) );
    m_connection->open();

    // TODO make synchronous properly
    while ( ! m_connection->xmppConnection() ) {
        QCoreApplication::processEvents();
    }

    m_browserModel->addConnection(*static_cast<QInfinity::XmlConnection*>(m_connection->xmppConnection()), "Test connection");
    kDebug() << "connection status:" << m_connection->xmppConnection()->status() << QInfinity::XmlConnection::Open;

    QInfinity::Browser* browser = m_browserModel->browsers().first();
    while ( browser->connectionStatus() != INFC_BROWSER_CONNECTED ) {
        QCoreApplication::processEvents();
    }

    QInfinity::BrowserIter iter(*browser);
    kDebug() << "connection root path:" << iter.path();

    if ( ! iter.isExplored() ) {
        kDebug() << "exploring iter";
        InfcExploreRequest* request = iter.explore();
        while ( INFC_IS_EXPLORE_REQUEST(request) && ! infc_explore_request_get_finished(INFC_EXPLORE_REQUEST(request)) ) {
            kDebug() << "waiting for exploration";
            QCoreApplication::processEvents();
        }
    }
    iter.child();

    do {
        UDSEntry entry;
        entry.insert( KIO::UDSEntry::UDS_URL, iter.path() );
        entry.insert( KIO::UDSEntry::UDS_NAME, iter.name() );
        entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG );
        kDebug() << "listing" << iter.path();
        listEntry(entry, false);
    } while ( iter.next() );

    listEntry(UDSEntry(), true);
    finished();


//     QString title;
//     QString section;
//
//     if ( !parseUrl(url.path(), title, section) ) {
//         error( KIO::ERR_MALFORMED_URL, url.url() );
//         return;
//     }
//
//     // stat() and listDir() declared that everything is an html file.
//     // However we can list man: and man:(1) as a directory (e.g. in dolphin).
//     // But we cannot list man:ls as a directory, this makes no sense (#154173)
//
//     if (!title.isEmpty() && title != "/") {
//     error(KIO::ERR_IS_FILE, url.url());
//         return;
//     }
//
//     UDSEntryList uds_entry_list;
//
//     if (section.isEmpty()) {
//         for (QStringList::ConstIterator it = section_names.constBegin(); it != section_names.constEnd(); ++it) {
//             UDSEntry     uds_entry;
//
//             QString name = "man:/(" + *it + ')';
//             uds_entry.insert( KIO::UDSEntry::UDS_NAME, sectionName( *it ) );
//             uds_entry.insert( KIO::UDSEntry::UDS_URL, name );
//             uds_entry.insert( KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR );
//
//             uds_entry_list.append( uds_entry );
//         }
//     }
//
//     QStringList list = findPages( section, QString(), false );
//
//     QStringList::Iterator it = list.begin();
//     QStringList::Iterator end = list.end();
//
//     for ( ; it != end; ++it ) {
//         stripExtension( &(*it) );
//
//         UDSEntry     uds_entry;
//         uds_entry.insert( KIO::UDSEntry::UDS_NAME, *it );
//         uds_entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG);
//         uds_entry.insert(KIO::UDSEntry::UDS_MIME_TYPE, QString::fromLatin1("text/html"));
//         uds_entry_list.append( uds_entry );
//     }
//
//     listEntries( uds_entry_list );
//     finished();
}
