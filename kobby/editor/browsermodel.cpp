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

#include <libinfinitymm/client/clientbrowseriter.h>

#include "browsermodel.h"
#include "browseritem.h"

#include <libqinfinitymm/connection.h>

#include <KDebug>

#include "browsermodel.moc"

namespace Kobby
{

BrowserModel::BrowserModel( QObject *parent )
    : QInfinity::BrowserModel( parent )
{
}

void BrowserModel::addConnection( QInfinity::Connection &connection )
{
    QInfinity::BrowserConnectionItem *item = new BrowserConnectionItem( connection, this );
    addConnectionItem( *item );
}

void BrowserModel::addNode( const Infinity::ClientBrowserIter &iter )
{
    Infinity::ClientBrowserIter t_iter = iter;
    QInfinity::BrowserNodeItem *nodeItem;

    if( t_iter.isDirectory() )
        nodeItem = new BrowserFolderItem( iter, this );
    else
        nodeItem = new BrowserNoteItem( iter, this );

    addNodeItem( *nodeItem );
}

}

