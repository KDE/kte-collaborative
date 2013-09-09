/*
 * Copyright 2013 Sven Brauch <svenbrauch@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "infinotenotifier.h"

#include "common/itemfactory.h"
#include "common/connection.h"
#include "common/utils.h"
#include "version.h"
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/xmppconnection.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/init.h>

#include "kpluginfactory.h"

#include <QDebug>
#include <QSharedPointer>
#include <QApplication>

#include <kdirnotify.h>
#include <KDE/KUrl>
#include <KDE/KDebug>
#include <KDE/KNotification>
#include <KDE/KLocalizedString>
#include <KDE/KMimeType>
#include <KDE/KIconLoader>
#include <KDE/KRun>
#include <KAboutData>
#include <KUniqueApplication>
#include <KCmdLineArgs>

int main(int argc, char **argv) {
    KAboutData about( "infinotenotifier", "kte-collaborative",
                      ki18n("infinotenotifier"), KTECOLLAB_VERSION_STRING,
                      ki18n("Notification helper for Collaborative Editing"), KAboutData::License_GPL_V2 );
    KCmdLineArgs::init(argc, argv, &about);
    KUniqueApplication a;
    Kobby::InfinoteNotifier notifier;
    if ( ! KUniqueApplication::start() ) {
        kDebug() << "notifier is already running";
        return 0;
    }
    return a.exec();
}

namespace Kobby {

Host hostForUrl(const KUrl& url) {
    return Host(url.host(), url.port());
}

InfinoteNotifier::InfinoteNotifier(QObject* parent)
    : QObject(parent)
    , m_notifyIface(new OrgKdeKDirNotifyInterface(QString(), QString(), QDBusConnection::sessionBus(), this))
    , m_browserModel(0)
{
    kDebug() << "Loaded module";
    QInfinity::init();
    connect(m_notifyIface, SIGNAL(enteredDirectory(QString)), SLOT(enteredDirectory(QString)));
    connect(m_notifyIface, SIGNAL(leftDirectory(QString)), SLOT(leftDirectory(QString)));
}

InfinoteNotifier::~InfinoteNotifier()
{
    kDebug() << "unloaded module";
    m_browserModel.clear();
}

void InfinoteNotifier::ensureInWatchlist(const QString& url_)
{
    KUrl url(url_);
    if ( m_watchedUrls.contains(url) ) {
        return;
    }
    if ( ! m_browserModel ) {
        m_browserModel = QSharedPointer<QInfinity::BrowserModel>(new QInfinity::BrowserModel(this));
        m_browserModel->setItemFactory(new Kobby::ItemFactory(this));
    }
    m_watchedUrls.insert(url);
    Host host(url.host(), url.port());
    bool haveConnectionForHost = m_connectionHostMap.values().contains(host);
    if ( haveConnectionForHost ) {
        kDebug() << "exploring" << url.url();
        IterLookupHelper* helper = new IterLookupHelper(url.path(), m_hostBrowserMap[host]);
        helper->setDeleteOnFinish();
        helper->setExploreResult();
        helper->begin();
        return;
    }
    // We do not handle errors here at all. If the connection fails, it'll not be watched.
    kDebug() << "creating connection for" << url;
    Kobby::Connection* conn = new Kobby::Connection(host.hostname, host.port, QString(), this);
    QObject::connect(conn, SIGNAL(ready(Connection*)), this, SLOT(connectionReady(Connection*)));
    QObject::connect(conn, SIGNAL(error(Connection*,QString)), SLOT(connectionError(Connection*,QString)));
    QObject::connect(conn, SIGNAL(disconnecting(Connection*)), SLOT(connectionDisconnected(Connection*)));
    QObject::connect(conn, SIGNAL(disconnected(Connection*)), SLOT(connectionDisconnected(Connection*)));
    conn->prepare();
}

void InfinoteNotifier::connectionDisconnected(Connection* connection)
{
    QInfinity::ConnectionItem* item = m_connectionItemMap[connection->xmppConnection()];
    Host host = m_connectionHostMap[item];
    if ( ! item || ! host.isValid() ) {
        return;
    }
    if ( ! m_browserModel ) {
        // destroying module
        return;
    }
    foreach ( const KUrl& url, m_watchedUrls ) {
        if ( hostForUrl(url) == host ) {
            m_watchedUrls.remove(url);
        }
    }
    m_browserModel->removeRows(item->index().row(), 1, QModelIndex());
    m_hostBrowserMap.take(host);
    m_connectionHostMap.take(item);
    m_connectionItemMap.take(connection->xmppConnection());
}

void InfinoteNotifier::connectionError(Connection* , QString error)
{
    kDebug() << "connection error:" << error;
}

void InfinoteNotifier::connectionReady(Connection* conn)
{
    kDebug() << "connection ready:" << conn;
    if ( ! conn || ! conn->xmppConnection() ) {
        return;
    }

    kDebug() << "adding connection";
    QInfinity::XmlConnection* xmlConnection = static_cast<QInfinity::XmlConnection*>(conn->xmppConnection());
    QInfinity::ConnectionItem* connItem = m_browserModel->addConnection(xmlConnection,
        conn->host().hostname + QString::number(conn->host().port)
    );

    QInfinity::Browser* browser = connItem->browser();
    m_connectionHostMap.insert(connItem, conn->host());
    m_connectionItemMap.insert(xmlConnection, connItem);
    m_hostBrowserMap.insert(conn->host(), browser);

    kDebug() << "opening connection";
    conn->open();

    connect(browser, SIGNAL(connectionEstablished(const QInfinity::Browser*)),
            this, SLOT(connectionEstablished(const QInfinity::Browser*)));
    connect(browser, SIGNAL(nodeAdded(BrowserIter)),
            this, SLOT(itemAdded(BrowserIter)), Qt::UniqueConnection);
    connect(browser, SIGNAL(nodeRemoved(BrowserIter)),
            this, SLOT(itemRemoved(BrowserIter)), Qt::UniqueConnection);
}

void InfinoteNotifier::connectionEstablished(const QInfinity::Browser* browser_)
{
    QInfinity::Browser* browser = const_cast<QInfinity::Browser*>(browser_); // sorry
    // Ensure all wateched directories are explored
    foreach ( const KUrl& watched, m_watchedUrls ) {
        const Host host = hostForUrl(watched);
        if ( host == m_connectionHostMap[m_connectionItemMap[browser->connection()]] ) {
            IterLookupHelper* helper = new IterLookupHelper(watched.path(), browser);
            helper->setDeleteOnFinish();
            helper->setExploreResult();
            helper->begin();
            connect(browser, SIGNAL(connectionEstablished(const QInfinity::Browser*)),
                    helper, SLOT(begin()));
        }
    }
}

void InfinoteNotifier::itemAdded(BrowserIter iter)
{
    kDebug() << "item added";
    QInfinity::BrowserIter copy(iter);
    if ( copy.parent() && infc_browser_iter_get_explore_request(copy.infBrowser(), copy.infBrowserIter()) ) {
        // the directory is just being explored, so the added signal is not a "real" one
        return;
    }
    QInfinity::XmlConnection* connection = iter.browser()->connection();
    if ( ! connection
         || iter.browser()->connectionStatus() != INFC_BROWSER_CONNECTED
         || connection->status() != QInfinity::XmlConnection::Open
         || ! m_connectionItemMap.contains(connection) )
    {
        return;
    }
    QInfinity::ConnectionItem* connItem = m_connectionItemMap[connection];
    const Host& host = m_connectionHostMap[connItem];
    bool haveAddedFile = false;
    foreach ( const KUrl& url, m_watchedUrls ) {
        // There might be multiple queued notifications for different formats of the
        // same URL. However, a notification will only show for one of them, since
        // the files added are only attached to one of them.
        if ( hostForUrl(url) == host ) {
            kDebug() << "queuing for update:" << url;
            QueuedNotification* item = m_notifyQueue.insertOrUpdateUrl(url.url(), this);
            if ( ! iter.isDirectory() && ! haveAddedFile ) {
                // do not show notifications for creting directories, those are worthless
                item->addedFiles.insert(iter.path());
                haveAddedFile = true;
            }
        }
    }
}

void InfinoteNotifier::itemRemoved(BrowserIter iter)
{
    kDebug() << "item removed";
    itemAdded(iter);
}

void InfinoteNotifier::enteredDirectory(QString path)
{
    if ( ! path.startsWith("inf") ) {
        return;
    }
    kDebug() << "entered directory" << path;
    ensureInWatchlist(path);
    kDebug() << "have watched:" << m_watchedUrls;
}

void InfinoteNotifier::leftDirectory(QString path)
{
    if ( ! path.startsWith("inf") ) {
        return;
    }
    kDebug() << "left directory" << path;
    m_watchedUrls.remove(path);
    kDebug() << "have watched:" << m_watchedUrls;
}

void InfinoteNotifier::notificationFired()
{
    QueuedNotification* notification = qobject_cast<QueuedNotification*>(QObject::sender());
    OrgKdeKDirNotifyInterface::emitFilesAdded(notification->url);
    m_notifyQueue.remove(notification);
    KUrl url(notification->url);
    if ( ! notification->addedFiles.isEmpty() ) {
        const int count = notification->addedFiles.count();
        QString typeString;
        KNotification* message = new KNotification("fileShared");
        if ( count == 1 ) {
            typeString = *notification->addedFiles.begin();
            message->setText(i18nc("%1 is a file name", "%1 was added for collaborative editing.", typeString));
            message->setActions(QStringList(i18n("Edit")));
            KUrl fullUrl(url);
            fullUrl.setPath(typeString);
            message->setProperty("openFile", fullUrl.url());
            // Use the file's mime type icon
        }
        else {
            message->setText(i18n("Some files were added for collaborative editing."));
            message->setActions(QStringList(i18n("Show folder")));
            KUrl folder(notification->url);
            folder.setPath("/");
            message->setProperty("openFolder", folder.url());
            // Use a folder icon
            typeString = "/";
        }
        KIconLoader loader;
        message->setPixmap(loader.loadMimeTypeIcon(KMimeType::findByPath(typeString)->iconName(),
                                                   KIconLoader::Dialog));
        message->setComponentData(KComponentData("infinotenotifier"));
        connect(message, SIGNAL(action1Activated()), SLOT(messageActionActivated()));
        message->sendEvent();
    }
    delete notification;
}

void InfinoteNotifier::messageActionActivated()
{
    QObject* message = QObject::sender();
    const QString openFile = message->property("openFile").toString();
    const QString openFolder = message->property("openFolder").toString();
    if ( ! openFile.isEmpty() ) {
        tryOpenDocumentWithDialog(KUrl(openFile));
    }
    else if ( ! openFolder.isEmpty() ) {
        // Unfortunately, we must run dolphin instead of the user's default file
        // manager, since the latter is not guaranteed to support KIO.
        KRun::runCommand("dolphin " + openFolder, 0);
    }
    if ( KNotification* notification = qobject_cast<KNotification*>(message) ) {
        // the cast should never fail, but since this a kded module extra guards don't hurt
        notification->close();
    }
}

QueuedNotification::QueuedNotification(const QString& notifyUrl, int msecs, QObject* parent)
    : QObject(parent)
    , url(notifyUrl)
    , timer(new QTimer)
{
    connect(timer, SIGNAL(timeout()), this, SIGNAL(fired()));
    connect(timer, SIGNAL(timeout()), this, SLOT(deleteLater()));
    timer->start(msecs);
}

QueuedNotification* InfinoteNotifier::QueuedNotificationSet::insertOrUpdateUrl(const QString& notifyUrl,
                                                                               InfinoteNotifier* parent, int msecs)
{
    foreach ( QueuedNotification* item, *this ) {
        if ( item->url == notifyUrl ) {
            // The item is already queued, update it
            item->timer->start(msecs);
            return item;
        }
    }
    // Otherwise, enqueue the item.
    QueuedNotification* queued = new QueuedNotification(notifyUrl, msecs, parent);
    connect(queued, SIGNAL(fired()), parent, SLOT(notificationFired()));
    insert(queued);
    return queued;
}

}

#include "infinotenotifier.moc"
