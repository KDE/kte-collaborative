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

#ifndef KOBBY_BROWSERITEM_H
#define KOBBY_BROWSERITEM_H

#include <libqinfinitymm/browseritem.h>

namespace Infinity
{
    class ClientBrowserIter;
}

namespace QInfinity
{
    class Connection;
}

namespace Kobby
{

class BrowserConnectionItem
    : public QInfinity::BrowserConnectionItem
{

    public:
        BrowserConnectionItem( QInfinity::Connection &connection,
            QObject *parent = 0 );

    protected:
        void onConnected();
        void onConnecting();
        void onDisconnected();

};

class BrowserFolderItem
    : public QInfinity::BrowserFolderItem
{

    public:
        BrowserFolderItem( const Infinity::ClientBrowserIter &iter,
            QObject *parent = 0 );
    
};

class BrowserNoteItem
    : public QInfinity::BrowserNoteItem
{

    public:
        BrowserNoteItem( const Infinity::ClientBrowserIter &iter,
            QObject *parent = 0 );

};

}

#endif

