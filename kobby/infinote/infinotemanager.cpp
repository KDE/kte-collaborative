#include <libinfinitymm/init.h>
#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>

#include "infinotemanager.h"

#include <KDebug>

#include <qinfinitymm/qtio.h>

#include <gnutls/gnutls.h>
#include <gsasl.h>

namespace Kobby
{

InfinoteManager::InfinoteManager( QObject *parent )
    : QObject( parent )
    , gnutls_cred( 0 )
    , gsasl_context( 0 )
{
    Infinity::init();
    
    io = new Infinity::QtIo;
    connectionManager = new Infinity::ConnectionManager();
}

InfinoteManager::~InfinoteManager()
{
    // Release remaining connections
    delete io;
}

Infinity::QtIo &InfinoteManager::getIo() const
{
    return *io;
}

Infinity::ConnectionManager &InfinoteManager::getConnectionManager() const
{
    return *connectionManager;
}

QList<Connection*> &InfinoteManager::getConnections()
{
    return connections;
}

Connection &InfinoteManager::connectToHost( const QString &jid, const QString &name, const QString &host, unsigned int port )
{
    Connection *connection;
    connection = new Connection( *this, name, jid, host, port, this );

    addConnection( *connection );

    connection->getTcpConnection().open();

    return *connection;
}

void InfinoteManager::addConnection( Connection &connection )
{
    connections.append( &connection );
    emit( connectionAdded( connection ) );
}

void InfinoteManager::removeConnection( Connection &connection )
{
    QList<Connection*>::iterator itr;
    Connection *local_conn = 0;

    for( itr = connections.begin(); itr != connections.end(); ++itr )
    {
        if( **itr == connection ) {
            local_conn = *itr;
            break;
        }
    }

    if( !local_conn )
    {
        kDebug() << "Couldnt find connection " << connection.getName();
        return;
    }

    connections.removeAll( local_conn );
    emit( connectionRemoved( connection ) );

    delete local_conn;
}

Infinity::XmppConnection &InfinoteManager::createXmppConnection( const QString &jid, const QString &host, unsigned int port )
{
    const char *hostname = host.toAscii();

    Infinity::IpAddress address( hostname );
    
    // Create TcpConnection and XmppConnection
    Infinity::TcpConnection *tcpConnection = new Infinity::TcpConnection( *io,
        address, port
    );
    Infinity::XmppConnection *xmppConnection = new Infinity::XmppConnection(
        *tcpConnection, Infinity::XMPP_CONNECTION_CLIENT, jid.toAscii(), gnutls_cred, gsasl_context
    );
    
    return *xmppConnection;
}

} // namespace Kobby
