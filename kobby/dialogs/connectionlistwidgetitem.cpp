#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>

#include <KIcon>

#include "connectionlistwidgetitem.h"

namespace Kobby
{

ConnectionListWidgetItem::ConnectionListWidgetItem( Connection &conn, QListWidget *parent )
    : QObject( parent)
    , QListWidgetItem( parent, QListWidgetItem::UserType )
    , connection( &conn )
    , has_connected( false )
{
    setDisplay();

    connect( connection, SIGNAL( statusChanged( int ) ), this, SLOT( slotStatusChanged( int ) ) );
}

void ConnectionListWidgetItem::setDisplay()
{
    QString statusLine;

    switch( connection->getTcpConnection().property_status() )
    {
        case Infinity::TCP_CONNECTION_CONNECTING:
            setIcon( KIcon( "network-disconnect.png" ) );
            statusLine = "Connecting...";
            break;
        case Infinity::TCP_CONNECTION_CONNECTED:
            has_connected = true;
            setIcon( KIcon( "network-connect.png" ) );
            statusLine = "Connected.";
            break;
        case Infinity::TCP_CONNECTION_CLOSED:
            if( !has_connected )
                statusLine = "Could not connect to server.";
            else
                statusLine = "Closed.";
            setIcon( KIcon( "network-disconnect.png" ) );
    }

    setText( connection->getName() + "\n" 
        + statusLine
    );
}

Connection &ConnectionListWidgetItem::getConnection()
{
    return *connection;
}

void ConnectionListWidgetItem::slotStatusChanged( int status )
{
    Q_UNUSED( status )
    setDisplay();
}

}
