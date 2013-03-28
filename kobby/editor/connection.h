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

#ifndef KOBBY_CONNECTION_H
#define KOBBY_CONNECTION_H

#include <QObject>

#include <glib.h>

class QHostInfo;

namespace QInfinity
{
    class TcpConnection;
    class XmppConnection;
}

namespace Kobby
{

/**
 * @brief Ties connection/creation monitoring to simple interface.
 */
class Connection
    : public QObject
{
    Q_OBJECT

    public:
        Connection( const QString &hostname,
            unsigned int port,
            QObject *parent = 0 );
        ~Connection();

        void open();
        QString name() const;
        QInfinity::XmppConnection *xmppConnection() const;

    Q_SIGNALS:
        void connecting( Connection *conn );
        void connected( Connection *conn );
        void disconnecting( Connection *conn );
        void disconnected( Connection *conn );
        void error( Connection *conn, QString message );

    private Q_SLOTS:
        void slotHostnameLookedUp( const QHostInfo &hostInfo );
        void slotStatusChanged();
        void slotError( const GError *err );

    private:
        QString m_hostname;
        unsigned int m_port;
        QInfinity::TcpConnection *m_tcpConnection;
        QInfinity::XmppConnection *m_xmppConnection;

};

}

#endif

