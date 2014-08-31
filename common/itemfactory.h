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

#ifndef KOBBY_ITEMFACTORY_H
#define KOBBY_ITEMFACTORY_H

#include "ktecollaborative_export.h"

#include <libqinfinity/browseritemfactory.h>

class KIcon;
class QObject;
class QString;

namespace QInfinity
{
    class XmlConnection;
    class Browser;
    class BrowserIter;
}

namespace Kobby
{

class Connection;

/**
 * Subclass so we can store connections with their item
 */
class KTECOLLABORATIVECOMMON_EXPORT ConnectionItem
    : public QInfinity::ConnectionItem
{

    public:
        ConnectionItem( QInfinity::XmlConnection &conn,
            QInfinity::Browser &browser,
            const KIcon &icon,
            const QString &text );
        ~ConnectionItem();

        void setConnection( Connection *conn );

    private:
        Connection *m_conn;

};



class KTECOLLABORATIVECOMMON_EXPORT ItemFactory
    : public QInfinity::BrowserItemFactory
{

    public:
        ItemFactory( QObject *parent = 0 );

        QInfinity::NodeItem *createRootNodeItem( const QInfinity::BrowserIter &iter );
        QInfinity::NodeItem *createNodeItem( const QInfinity::BrowserIter &iter );
        QInfinity::ConnectionItem *createConnectionItem( QInfinity::XmlConnection &conn,
            QInfinity::Browser &browser,
            const QString &name );

};

}

#endif

