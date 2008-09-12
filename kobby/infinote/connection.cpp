#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>
#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientexplorerequest.h>

#include <qinfinitymm/qtio.h>

#include "connection.h"
#include "infinotemanager.h"

#include <KDebug>

namespace Kobby
{

Connection::Connection( InfinoteManager &manager, const QString connectionName, Infinity::XmppConnection &connection, QObject *parent  )
    : QObject( parent )
    , infinoteManager( &manager )
    , name( connectionName )
    , xmppConnection( &connection )
    , tcpConnection( connection.property_tcp_connection().get_value() )
    , clientBrowser( new Infinity::ClientBrowser( infinoteManager->getIo(), *xmppConnection, infinoteManager->getConnectionManager() ) )
    , has_connected( false )
{
    xmppConnection->property_status().signal_changed().connect( sigc::mem_fun( this, &Connection::statusChangedCb ) );
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

InfinoteManager &Connection::getInfinoteManager() const
{
    return *infinoteManager;
}

int Connection::getStatus() const
{
    return xmppConnection->getStatus();
}

Infinity::ClientBrowser &Connection::getClientBrowser() const
{
    return *clientBrowser;
}

void Connection::statusChangedCb()
{
    kDebug() << "status changed.";

    switch( getStatus() )
    {
        case Infinity::XML_CONNECTION_OPEN:
            if ( !has_connected )
                has_connected = true;
    }

    emit( statusChanged( getStatus() ) );
}

}
