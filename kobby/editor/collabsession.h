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

#ifndef KOBBY_COLLABSESSION_H
#define KOBBY_COLLABSESSION_H

#include <QObject>

namespace Infinity
{
    class TextSession;
    class XmlConnection;
    class ConnectionManagerGroup;
}

namespace Kobby
{

class CollabSession
    : public QObject
{

    Q_OBJECT
    public:
        CollabSession( Infinity::TextSession &infTextSession,
            QObject *parent = 0 );

        Infinity::TextSession &textSession() const;

    Q_SIGNALS:
        void synchronizationBegin();
        void synchronizationComplete();

    private:
        void synchronizationBegin( Infinity::ConnectionManagerGroup *group, Infinity::XmlConnection *connection );
        void synchronizationComplete( Infinity::XmlConnection *connection );

        Infinity::TextSession *m_textSession;

};

}

#endif
