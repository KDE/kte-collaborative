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

#ifndef INFINOTENOTIFIER_H
#define INFINOTENOTIFIER_H

#include <QtDBus/QDBusContext>
#include <QSet>
#include <QSharedPointer>

#include <common/connection.h>

class QTimer;
namespace QInfinity {
    class BrowserModel;
    class ConnectionItem;
    class XmlConnection;
    class Connection;
    class BrowserIter;
    class Browser;
}
class KUrl;
class OrgKdeKDirNotifyInterface;

using Kobby::Connection;
using Kobby::Host;
using QInfinity::BrowserIter;

namespace Kobby {

unsigned int qHash(const Host& host)
{
    return qHash(host.hostname) + host.port;
}

/**
 * This class represents a notification in the queue.
 * It will emit a KDirNotify signal for the folder it is for, to make applications
 * displaying the folder reload it.
 * It might or might not display a popup message notifying the user, too; see notificationFired.
 */
class QueuedNotification : public QObject {
Q_OBJECT
public:
    /// Constructs (but does not enqueue) a new notification.
    QueuedNotification(const QString& notifyUrl, int msecs, QObject* parent = 0);
    /// URL (a folder!) this notification is for
    const QString url;
    /// Files which were added, for deciding whether to notify the user or not
    QSet<QString> addedFiles;
    /// Timer for displaying/emitting the notification, in order to group bulk operations
    QTimer* timer;

signals:
    /// just the timer's timeout()
    void fired();
};

/**
 * @brief Notification daemon for updating infinity directory views and sending notifications.
 * The infinity protocol sends push notifications when files are added or removed. The main
 * task of this kded module is to keep track of what directories are being displayed in a file
 * manager or other file view (e.g. kate "File system" side bar) and emit signals on the
 * KDirNotify interface when a file is added or removed in such a directory.
 * As a secondary task, it can also display popup notifications for the user, to tell that
 * someone has shared a new file.
 */
class InfinoteNotifier : public QObject
{
Q_OBJECT

public:
    InfinoteNotifier(QObject *parent = 0);
    virtual ~InfinoteNotifier();

private slots:
    /// Slots invoked when another application (e.g. dolphin) enters or leaves a directory
    void enteredDirectory(QString);
    void leftDirectory(QString);

    /// Handlers for the connections to the watched directories
    void connectionReady(Connection*);
    void connectionError(Connection*,QString);
    void connectionDisconnected(Connection*);
    void connectionEstablished(const QInfinity::Browser*);

    /// Handlers for the push notifications sent by the protocol
    void itemAdded(BrowserIter);
    void itemRemoved(BrowserIter);

    /// Called when a queued notification times out, i.e. should be displayed
    void notificationFired();
    /// Called when the button in one of the popup messages is clicked
    void messageActionActivated();

private:
    /// Ensures @p url is in the list of watched URLs.
    /// Items are only removed from the watchlist when a connection breaks, since
    /// establishing the connection is the most costly thing.
    void ensureInWatchlist(const QString& url);

    /// Clean all connections from the connections list which are broken,
    /// and remove the watched URLs for those
    void cleanupConnectionList();
    /// Cleanup one particular connection.
    void cleanupConnection(QInfinity::XmlConnection* conn);

private:
    OrgKdeKDirNotifyInterface* m_notifyIface;
    /// List of watched URLs to receive notifications via KDirNotify
    QSet<KUrl> m_watchedUrls;
    QSharedPointer<QInfinity::BrowserModel> m_browserModel;
    QMap<QInfinity::ConnectionItem*, Host> m_connectionHostMap;
    QMap<QInfinity::XmlConnection*, QInfinity::ConnectionItem*> m_connectionItemMap;
    QHash<Host, QInfinity::Browser*> m_hostBrowserMap;

    struct QueuedNotificationSet : public QSet<QueuedNotification*> {
        /// Adds @p notifyUrl to the list of URLs to send a notification for.
        /// @returns the QueuedNotification for @p notifyUrl. Add files to its addedFiles
        /// propery as appopriate.
        QueuedNotification* insertOrUpdateUrl(const QString& notifyUrl, Kobby::InfinoteNotifier* parent, int msecs = 4000);
    };
    friend struct QueuedNotificationSet;
    /// Set of notifications to be displayed when they time out
    QueuedNotificationSet m_notifyQueue;
};

}

#endif // INFINOTENOTIFIER_H
