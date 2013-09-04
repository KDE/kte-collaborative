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

#include <kdedmodule.h>
#include <common/connection.h>

namespace QInfinity {
    class BrowserModel;
    class ConnectionItem;
    class XmlConnection;
    class Connection;
    class BrowserIter;
}
class OrgKdeKDirNotifyInterface;

using Kobby::Connection;
using QInfinity::BrowserIter;

struct Host {
    Host(const QString& hostname, unsigned short port) : hostname(hostname), port(port) { };
    bool operator==(const Host& other) const { return hostname == other.hostname && port == other.port; };
    QString hostname;
    unsigned short port;
};

unsigned int qHash(const Host& host) {
    return qHash(host.hostname) + host.port;
};

class InfinoteNotifier : public KDEDModule, QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.InfinoteNotifier")

public:
    InfinoteNotifier(QObject *parent, const QVariantList &);

private slots:
    void enteredDirectory(QString);
    void leftDirectory(QString);
    void connectionReady(Connection*);
    void itemAdded(BrowserIter);
    void itemRemoved(BrowserIter);

private:
    void ensureInWatchlist(const QString& url);
    void removeFromWatchlist(const QString& url);

private:
    OrgKdeKDirNotifyInterface* m_notifyIface;
    QSet<QString> m_watchedUrls;
    QSharedPointer<QInfinity::BrowserModel> m_browserModel;
    QHash<Host, QInfinity::ConnectionItem*> m_hostConnectionMap;
    QHash<QInfinity::XmlConnection*, QInfinity::ConnectionItem*> m_connectionItemMap;
};

#endif // INFINOTENOTIFIER_H
