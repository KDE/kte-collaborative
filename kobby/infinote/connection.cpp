#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>
#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientexplorerequest.h>

#include <qinfinitymm/qtio.h>

#include "connection.h"
#include "infinotemanager.h"

namespace Kobby
{

Connection::Connection( InfinoteManager &manager, const QString connectionName, Infinity::XmppConnection &connection )
    : infinoteManager( &manager )
    , name( connectionName )
    , xmppConnection( &connection )
    , tcpConnection( connection.property_tcp_connection().get_value() )
    , has_connected( false )
{
    tcpConnection->property_status().signal_changed().connect( sigc::mem_fun( this, &Connection::slotStatusChanged ) );
}

#define DEL_IF_EXISTS( x ) if( x ) delete x;

Connection::~Connection()
{
    if( tcpConnection )
        tcpConnection->close();

    DEL_IF_EXISTS( xmppConnection )
    DEL_IF_EXISTS( tcpConnection )
}

#undef DEL_IF_EXISTS

bool Connection::operator==( const Connection &connection ) const
{
    if( ( &connection.getXmppConnection() == xmppConnection ) && ( &connection.getTcpConnection() == tcpConnection ) )
        return true;
    else
        return false;
}

bool Connection::operator!=( const Connection &connection ) const
{
    return !( connection == *this );
}

const QString &Connection::getName() const
{
    return name;
}

Infinity::XmppConnection &Connection::getXmppConnection() const
{
    return *xmppConnection;
}

Infinity::TcpConnection &Connection::getTcpConnection() const
{
    return *tcpConnection;
}

Infinity::ClientBrowser &Connection::getClientBrowser() const
{
    return *clientBrowser;
}

int Connection::getStatus() const
{
    return tcpConnection->property_status().get_value();
}

void Connection::slotStatusChanged()
{
    switch( getStatus() )
    {
        case Infinity::TCP_CONNECTION_CONNECTED:
            if ( !has_connected ) {
                has_connected = true;
                clientBrowser = new Infinity::ClientBrowser( infinoteManager->getIo(), getXmppConnection(), infinoteManager->getConnectionManager() );
            }
    }

    emit( statusChanged( tcpConnection->property_status().get_value() ) );
}

}
