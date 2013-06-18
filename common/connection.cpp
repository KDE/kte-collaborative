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

#include "connection.h"

#include <libqinfinity/ipaddress.h>
#include <libqinfinity/tcpconnection.h>
#include <libqinfinity/xmppconnection.h>

#include <QHostInfo>
#include <QHostAddress>
#include <QDebug>
#include <QApplication>

#include "connection.moc"
#include <kobby/kobbyplugin.h>

namespace Kobby
{

Connection::Connection( const QString &hostname,
    unsigned int port,
    QObject *parent )
    : QObject( parent )
    , m_hostname( hostname )
    , m_port( port )
    , m_tcpConnection( 0 )
    , m_xmppConnection( 0 )
    , m_connectionStatus(QInfinity::XmlConnection::Closed)
{
}

Connection::~Connection()
{
}

void Connection::prepare()
{
    if ( property("useSimulatedConnection").toBool() ) {
        m_xmppConnection = new QInfinity::XmppConnection( this );
        connect( m_xmppConnection, SIGNAL(statusChanged()),
            this, SLOT(slotStatusChanged()) );
        connect( m_xmppConnection, SIGNAL(error( const GError* )),
            this, SLOT(slotError( const GError* )) );
        emit ready( this );
    }
    else {
        QHostInfo::lookupHost( m_hostname, this,
            SLOT(slotHostnameLookedUp(const QHostInfo&)) );
    }
}

void Connection::open()
{
    if ( property("useSimulatedConnection").toBool() ) {
        return;
    }
    Q_ASSERT(m_tcpConnection && "you must call prepare() and wait for the ready() signal to be emitted before calling open()");
    m_tcpConnection->open();
}

QString Connection::name() const
{
    QString str = m_hostname + ":" + QString::number( m_port );
    return str;
}

QInfinity::XmppConnection *Connection::xmppConnection() const
{
    return m_xmppConnection;
}

void Connection::slotHostnameLookedUp( const QHostInfo &hostInfo )
{
    QList<QHostAddress> addresses = hostInfo.addresses();
    if( addresses.size() == 0 )
    {
        emit(error( this, "Host not found." ));
        return;
    }

    m_tcpConnection = new QInfinity::TcpConnection( QInfinity::IpAddress( addresses[0] ),
        m_port,
        this );

    m_xmppConnection = new QInfinity::XmppConnection( *m_tcpConnection,
        QInfinity::XmppConnection::Client,
#warning fixme
        "localhost",
        m_hostname,
        QInfinity::XmppConnection::PreferTls,
        0, 0, 0,
        this );

    connect( m_xmppConnection, SIGNAL(statusChanged()),
        this, SLOT(slotStatusChanged()) );
    connect( m_xmppConnection, SIGNAL(error( const GError* )),
        this, SLOT(slotError( const GError* )) );

    emit ready( this );
}

QInfinity::XmlConnection::Status Connection::status() const
{
    return m_connectionStatus;
}

void Connection::slotStatusChanged()
{
    m_connectionStatus = m_xmppConnection->status();
    emit statusChanged(this, m_connectionStatus);
    switch( m_xmppConnection->status() )
    {
        case QInfinity::XmlConnection::Opening:
            emit(connecting( this ));
            break;
        case QInfinity::XmlConnection::Closing:
            emit(disconnecting( this ));
            break;
        case QInfinity::XmlConnection::Open:
            emit(connected( this ));
            break;
        case QInfinity::XmlConnection::Closed:
            emit(disconnected( this ));
    }
}

void Connection::slotError( const GError *err )
{
    emit(error( this, QString(err->message) ));
}

}

