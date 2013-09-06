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
#include <common/itemfactory.h>
#include <common/connection.h>
#include <common/utils.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/xmppconnection.h>
#include <libqinfinity/browser.h>

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

namespace Kobby {

K_PLUGIN_FACTORY(InfinoteNotifierFactory, registerPlugin<InfinoteNotifier>();)
K_EXPORT_PLUGIN(InfinoteNotifierFactory("infinotenotifier"))

Host hostForUrl(const KUrl& url) {
    return Host(url.host(), url.port());
}

InfinoteNotifier::InfinoteNotifier(QObject* parent, const QVariantList& )
    : KDEDModule(parent)
    , QDBusContext()
    , m_notifyIface(new OrgKdeKDirNotifyInterface(QString(), QString(), QDBusConnection::sessionBus(), this))
    , m_browserModel(0)
{
    qDebug() << "Loaded module";
    connect(m_notifyIface, SIGNAL(enteredDirectory(QString)), SLOT(enteredDirectory(QString)));
    connect(m_notifyIface, SIGNAL(leftDirectory(QString)), SLOT(leftDirectory(QString)));
}

InfinoteNotifier::~InfinoteNotifier()
{
    qDebug() << "unloaded module";
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
        qDebug() << "exploring" << url.url();
        IterLookupHelper* helper = new IterLookupHelper(url.path(), m_hostBrowserMap[host]);
        helper->setDeleteOnFinish();
        helper->setExploreResult();
        helper->begin();
        return;
    }
    // We do not handle errors here at all. If the connection fails, it'll not be watched.
    qDebug() << "creating connection for" << url;
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
    m_browserModel->removeRows(item->index().row(), 1, QModelIndex());
    m_hostBrowserMap.take(host);
    m_connectionHostMap.take(item);
    m_connectionItemMap.take(connection->xmppConnection());
}

void InfinoteNotifier::connectionError(Connection* , QString error)
{
    qDebug() << "connection error:" << error;
}

void InfinoteNotifier::connectionReady(Connection* conn)
{
    qDebug() << "connection ready:" << conn;
    if ( ! conn || ! conn->xmppConnection() ) {
        return;
    }

    qDebug() << "adding connection";
    QInfinity::XmlConnection* xmlConnection = static_cast<QInfinity::XmlConnection*>(conn->xmppConnection());
    QInfinity::ConnectionItem* connItem = m_browserModel->addConnection(xmlConnection,
        conn->host().hostname + QString::number(conn->host().port)
    );

    QInfinity::Browser* browser = connItem->browser();
    m_connectionHostMap.insert(connItem, conn->host());
    m_connectionItemMap.insert(xmlConnection, connItem);
    m_hostBrowserMap.insert(conn->host(), browser);

    qDebug() << "opening connection";
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
    qDebug() << "item added";
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
    foreach ( const KUrl& url, m_watchedUrls ) {
        if ( hostForUrl(url) == host ) {
            qDebug() << "queuing for update:" << url;
            QueuedNotification* item = m_notifyQueue.insertOrUpdateUrl(url.url(), this);
            item->addedFiles.insert(iter.path());
        }
    }
}

void InfinoteNotifier::itemRemoved(BrowserIter iter)
{
    qDebug() << "item removed";
    itemAdded(iter);
}

void InfinoteNotifier::removeFromWatchlist(const QString& /*url*/)
{

}

void InfinoteNotifier::enteredDirectory(QString path)
{
    if ( ! path.startsWith("inf") ) {
        return;
    }
    qDebug() << "entered directory" << path;
    ensureInWatchlist(path);
    qDebug() << "have watched:" << m_watchedUrls;
}

void InfinoteNotifier::leftDirectory(QString path)
{
    if ( ! path.startsWith("inf") ) {
        return;
    }
    qDebug() << "left directory" << path;
    m_watchedUrls.remove(path);
    removeFromWatchlist(path);
    qDebug() << "have watched:" << m_watchedUrls;
}

void InfinoteNotifier::notificationFired()
{
    QueuedNotification* notification = qobject_cast<QueuedNotification*>(QObject::sender());
    qDebug() << "emitting changed:" << notification->url;
    OrgKdeKDirNotifyInterface::emitFilesAdded(notification->url);
    m_notifyQueue.remove(notification);
    KUrl url(notification->url);
    qDebug() << "files added:" << notification->addedFiles;
    if ( ! notification->addedFiles.isEmpty() ) {
        KNotification* message = new KNotification("fileShared");
        message->setText(i18n("A new file was added for collaborative editing."));
        KIconLoader loader;
        message->setPixmap(loader.loadMimeTypeIcon(KMimeType::findByPath(url.url())->iconName(),
                                                   KIconLoader::Dialog));
        message->setActions(QStringList(i18n("Open in editor")));
        message->sendEvent();
    }
    delete notification;
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
