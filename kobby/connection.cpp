#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>

#include "connection.h"

namespace Kobby
{

Connection::Connection( const QString connectionName, Infinity::XmppConnection &connection )
    : name( connectionName )
    , xmppConnection( &connection )
    , tcpConnection( connection.property_tcp_connection().get_value() )
{
    tcpConnection->property_status().signal_changed().connect( sigc::mem_fun( this, &Connection::slotStatusChanged ) );
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

void Connection::slotStatusChanged()
{
    emit( statusChanged( tcpConnection->property_status().get_value() ) );
}

}
