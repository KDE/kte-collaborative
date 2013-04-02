/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "itemfactory.h"
#include "connection.h"

#include <libqinfinity/browseriter.h>
#include <libqinfinity/browser.h>

#include <KIcon>

namespace Kobby
{

ConnectionItem::ConnectionItem( QInfinity::XmlConnection &conn,
    QInfinity::Browser &browser,
    const KIcon &icon,
    const QString &text )
    : QInfinity::ConnectionItem( conn, browser, icon, text )
    , m_conn( 0 )
{
}

ConnectionItem::~ConnectionItem()
{
    if( m_conn )
        delete m_conn;
}

void ConnectionItem::setConnection( Connection *conn )
{
    m_conn = conn;
}

ItemFactory::ItemFactory( QObject *parent )
    : QInfinity::BrowserItemFactory( parent )
{
}

QInfinity::NodeItem *ItemFactory::createRootNodeItem( const QInfinity::BrowserIter &iter )
{
    QInfinity::NodeItem *item;
    item = new QInfinity::NodeItem( iter, KIcon("folder.png"), "/" );
    return item;
}

QInfinity::NodeItem *ItemFactory::createNodeItem( const QInfinity::BrowserIter &iter )
{
    QInfinity::BrowserIter localItr = iter;

    QInfinity::NodeItem *item;
    if( localItr.isDirectory() )
        item = new QInfinity::NodeItem( localItr, KIcon("folder.png") );
    else
        item = new QInfinity::NodeItem( localItr, KIcon("text-x-generic.png") );
    return item;
}

QInfinity::ConnectionItem *ItemFactory::createConnectionItem( QInfinity::XmlConnection &conn,
    QInfinity::Browser &browser,
    const QString &name )
{
    return new Kobby::ConnectionItem( conn, browser, KIcon("network-connect.png"), name );
}

}

