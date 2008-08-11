#include <libinfinitymm/init.h>
#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>

#include <qinfinitymm/qtio.h>

#include "infinotemanager.h"

extern "C"
{
    #include <gnutls/gnutls.h>
    #include <gsasl.h>
}

namespace Kobby
{

InfinoteManager::InfinoteManager()
    : jid( "anonymous@localhost" )
    , gnutls_cred( 0 )
    , gsasl_context( 0 )
{
    Infinity::init();
    
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

    connection = new Connection( name, newXmppConnection( host, port ) );
    connections.append( connection );

    connection->getTcpConnection().open();

    return *connection;
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

Infinity::QtIo &InfinoteManager::getIo() const
{
    return *io;
}

const QString &InfinoteManager::getJid() const
{
    return jid;
}

void InfinoteManager::setJid( const QString &string )
{
    jid = string;
    emit( jidChanged( jid ) );
}

} // namespace Kobby
