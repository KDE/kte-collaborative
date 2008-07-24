#include <libinfinitymm/init.h>
#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>

#include <qlibinfinitymm/qtio.h>

#include "infinotemanager.h"

extern "C"
{
    #include <gnutls/gnutls.h>
    #include <gsasl.h>
}

namespace Kobby
{

InfinoteManager::InfinoteManager()
{
    Infinity::init();
    
    io = new Infinity::QtIo;
}

InfinoteManager::~InfinoteManager()
{
    // Release remaining connections
    
    delete io;
}

Infinity::XmppConnection &InfinoteManager::newXmppConnection( const QString &host, 
    unsigned int port,
    const char *jid,
    gnutls_certificate_credentials_t cred,
    Gsasl *sasl_context
)
{
    return newXmppConnection( host.toAscii().data(), port, jid, cred, sasl_context );
}

Infinity::XmppConnection &InfinoteManager::newXmppConnection( const char *host, 
    unsigned int port,
    const char *jid,
    gnutls_certificate_credentials_t cred,
    Gsasl *sasl_context
)
{
    Infinity::IpAddress address(host);
    
    // Create TcpConnection and XmppConnection
    Infinity::TcpConnection *tcpConnection = new Infinity::TcpConnection(*io,
        address, port);
    Infinity::XmppConnection *xmppConnection = new Infinity::XmppConnection(
        *tcpConnection, Infinity::XMPP_CONNECTION_CLIENT, jid, cred, sasl_context);
    
    connections.append(xmppConnection);
    
    return *xmppConnection;
}

} // namespace Kobby
