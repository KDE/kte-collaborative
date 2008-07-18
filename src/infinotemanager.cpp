#include <libinfinitymm/init.h>
#include <libinfinitymm/common/tcpconnection.h>

#include "infinotemanager.h"

namespace Kobby
{

InfinoteManager::InfinoteManager()
{
    Infinity::init();
}

InfinoteManager::~InfinoteManager()
{
    // Release remaining connections
}

Infinity::XmppConnection &InfinoteManager::newXmppConnection( const char *host, 
    unsigned int port,
    const char *jid,
    gnutls_certificate_credentials_t cred,
    Gsasl *sasl_context
)
{
    Q_UNUSED( host );
    Q_UNUSED( port );
    Q_UNUSED( jid );
    Q_UNUSED( cred );
    Q_UNUSED( sasl_context );
    // Create TcpConnection and XmppConnection
    // Add XmppConnection to connections
}

} // namespace Kobby
