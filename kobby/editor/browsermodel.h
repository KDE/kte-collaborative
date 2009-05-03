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

#ifndef KOBBY_BROWSERMODEL_H
#define KOBBY_BROWSERMODEL_H

#include <libqinfinitymm/browsermodel.h>

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

/**
 * @brief Model containing all connections and nodes on each connection.
 */
class BrowserModel
    : public QInfinity::BrowserModel
{
    Q_OBJECT

    public:
        BrowserModel( QObject *parent = 0 );

    protected Q_SLOTS:
        /**
         * We are overriding this to make use of the KDE icon system.
         */
        void addConnection( QInfinity::Connection &connection );
        /**
         * We are overriding this to make use of the KDE icon system.
         */
        void addNode( const Infinity::ClientBrowserIter &iter );

};

}

#endif

