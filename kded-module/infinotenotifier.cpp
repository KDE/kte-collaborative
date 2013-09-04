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
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/xmppconnection.h>
#include <libqinfinity/browser.h>

#include "kpluginfactory.h"

#include <QDebug>
#include <QSharedPointer>

#include <kdirnotify.h>
#include <KDE/KUrl>

K_PLUGIN_FACTORY(InfinoteNotifierFactory, registerPlugin<InfinoteNotifier>();)
K_EXPORT_PLUGIN(InfinoteNotifierFactory("infinotenotifier"))

using namespace Kobby;

InfinoteNotifier::InfinoteNotifier(QObject* parent, const QVariantList& )
    : KDEDModule(parent)
    , m_notifyIface(new OrgKdeKDirNotifyInterface(QString(), QString(), QDBusConnection::sessionBus(), this))
    , m_browserModel(0)
{
    qDebug() << "Loaded module";
    connect(m_notifyIface, SIGNAL(enteredDirectory(QString)), SLOT(enteredDirectory(QString)));
    connect(m_notifyIface, SIGNAL(leftDirectory(QString)), SLOT(leftDirectory(QString)));
}

void InfinoteNotifier::ensureInWatchlist(const QString& url_)
{
    KUrl url(url_);
    if ( ! m_browserModel ) {
        m_browserModel = QSharedPointer<QInfinity::BrowserModel>(new QInfinity::BrowserModel(this));
    }
    // We do not handle errors here at all. If the connection fails, it'll not be watched.
    QSharedPointer<Kobby::Connection> conn(new Kobby::Connection(url.host(), url.port(), QString(), this));
    m_browserModel->setItemFactory(new Kobby::ItemFactory(this));
    QInfinity::XmlConnection* xmlConnection = static_cast<QInfinity::XmlConnection*>(conn->xmppConnection());
    QInfinity::ConnectionItem* connItem = m_browserModel->addConnection(xmlConnection, url.host() + QString::number(url.port()));
    QObject::connect(conn.data(), SIGNAL(ready(Connection*)), this, SLOT(connectionReady(Connection*)));
    conn->prepare();

    m_hostConnectionMap.insert(Host(url.host(), url.port()), connItem);
    m_connectionItemMap.insert(xmlConnection, connItem);
}

void InfinoteNotifier::connectionReady(Connection* conn)
{
    if ( ! conn || ! conn->xmppConnection() || ! m_connectionItemMap.contains(conn->xmppConnection()) ) {
        return;
    }
    QInfinity::XmlConnection* xmlConnection = static_cast<QInfinity::XmlConnection*>(conn->xmppConnection());
    conn->open();
    QInfinity::Browser* browser = m_connectionItemMap.value(xmlConnection)->browser();
    if ( ! browser ) {
        return;
    }

    connect(browser, SIGNAL(nodeAdded(BrowserIter)),
            this, SLOT(itemAdded(BrowserIter)), Qt::UniqueConnection);
    connect(browser, SIGNAL(nodeRemoved(BrowserIter)),
            this, SLOT(itemRemoved(BrowserIter)), Qt::UniqueConnection);
}

void InfinoteNotifier::itemAdded(BrowserIter )
{
    qDebug() << "item added";
}

void InfinoteNotifier::itemRemoved(BrowserIter )
{
    qDebug() << "item removed";
}

void InfinoteNotifier::enteredDirectory(QString path)
{
    if ( ! path.startsWith("inf") ) {
        return;
    }
    qDebug() << "entered directory" << path;
    m_watchedUrls.insert(path);
    qDebug() << "have watched:" << m_watchedUrls;
}

void InfinoteNotifier::leftDirectory(QString path)
{
    if ( ! path.startsWith("inf") ) {
        return;
    }
    qDebug() << "left directory" << path;
    m_watchedUrls.remove(path);
    qDebug() << "have watched:" << m_watchedUrls;
}

#include "infinotenotifier.moc"
