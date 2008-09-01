#include <libinfinitymm/init.h>
#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>

#include "infinotemanager.h"

#include <qinfinitymm/qtio.h>

#include <gnutls/gnutls.h>
#include <gsasl.h>

namespace Kobby
{

InfinoteManager::InfinoteManager()
    : jid( "anonymous@localhost" )
    , gnutls_cred( 0 )
    , gsasl_context( 0 )
{
    Infinity::init();
    
    connectionManager = new Infinity::ConnectionManager();
    io = new Infinity::QtIo;
}

InfinoteManager::~InfinoteManager()
{
    // Release remaining connections
    
    delete io;
}

Connection &InfinoteManager::connectToHost( const QString &name, const QString &host, unsigned int port )
{
    Connection *connection;

    connection = new Connection( *this, name, newXmppConnection( host, port ) );
    connections.append( connection );

    connection->getTcpConnection().open();

    return *connection;
}

Infinity::QtIo &InfinoteManager::getIo() const
{
    return *io;
}

const QString &InfinoteManager::getJid() const
{
    return jid;
}

Infinity::ConnectionManager &InfinoteManager::getConnectionManager() const
{
    return *connectionManager;
}

void InfinoteManager::setJid( const QString &string )
{
    jid = string;
    emit( jidChanged( jid ) );
}

Infinity::XmppConnection &InfinoteManager::newXmppConnection( const QString &host, unsigned int port )
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
