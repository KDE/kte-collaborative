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
#include "kobbycommon_export.h"
#include <libqinfinity/xmlconnection.h>

#include <QObject>
#include <QDebug>

#include <glib.h>

class QHostInfo;

namespace QInfinity
{
    class TcpConnection;
    class XmppConnection;
}

namespace Kobby
{

struct Host {
    Host(const QString& hostname, int port) : hostname(hostname), port(port == -1 ? 6523 : port) { };
    Host() : port(0) { };
    bool operator==(const Host& other) const { return hostname == other.hostname && port == other.port; };
    bool isValid() { return ! hostname.isNull() && port != 0; };
    QString hostname;
    int port;
};

/**
 * @brief Ties connection/creation monitoring to simple interface.
 */
class KOBBYCOMMON_EXPORT Connection
    : public QObject
{
    Q_OBJECT

    public:
        Connection( const QString &hostname,
            unsigned int port,
            const QString& name_,
            QObject *parent = 0);
        ~Connection();

        // Prepares the connection by looking up the host name and populating
        // the internals as soon as it is looked up.
        void prepare();
        // Opens a prepared connection. You must call prepare() and wait for the ready()
        // signal before calling this.
        void open();
        QString name() const;
        // Returns the xmpp connection if called after ready()
        // was emitted, or 0 otherwise.
        QInfinity::XmppConnection *xmppConnection() const;
        // Returns the status of the connection.
        QInfinity::XmlConnection::Status status() const;
        // Returns the host this connection is for.
        Host host() const;

    Q_SIGNALS:
        void connecting( Connection *conn );
        void connected( Connection *conn );
        void disconnecting( Connection *conn );
        void disconnected( Connection *conn );
        // Emitted additionally to the above four signals, for convenience.
        void statusChanged( Connection *conn, QInfinity::XmlConnection::Status status );
        void error( Connection *conn, QString message );
        // This signal is emitted when the connection is ready to be opened,
        // i.e. when the host name was looked up and everything is set up.
        // When this signal is emitted, xmppConnection() becomes valid.
        void ready( Connection* conn );

    private Q_SLOTS:
        void slotHostnameLookedUp( const QHostInfo &hostInfo );
        void slotStatusChanged();
        void slotError( const GError *err );

    private:
        Host m_host;
        // A unique identifier for a particular host/port combination, usually host:port.
        const QString m_name;
        QInfinity::XmlConnection::Status m_connectionStatus;
        QInfinity::TcpConnection *m_tcpConnection;
        QInfinity::XmppConnection *m_xmppConnection;

};

}

#endif

