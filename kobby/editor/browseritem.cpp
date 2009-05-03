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
#include "browseritem.h"

#include <libqinfinitymm/connection.h>

#include <KIcon>

namespace Kobby
{

BrowserConnectionItem::BrowserConnectionItem( QInfinity::Connection &connection,
    QObject *parent )
    : QInfinity::BrowserConnectionItem( connection, parent )
{
}

void BrowserConnectionItem::onConnected()
{
    setIcon( KIcon( "network-connect.png" ) );
}

void BrowserConnectionItem::onConnecting()
{
    setIcon( KIcon( "network-disconnect.png" ) );
}

void BrowserConnectionItem::onDisconnected()
{
    setIcon( KIcon( "network-disconnect.png" ) );
}

BrowserFolderItem::BrowserFolderItem( const Infinity::ClientBrowserIter &iter,
    QObject *parent )
    : QInfinity::BrowserFolderItem( iter, parent )
{
}

BrowserNoteItem::BrowserNoteItem( const Infinity::ClientBrowserIter &iter,
    QObject *parent )
    : QInfinity::BrowserNoteItem( iter, parent )
{
}

}

