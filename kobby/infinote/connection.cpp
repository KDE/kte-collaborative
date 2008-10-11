#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>
#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientexplorerequest.h>
#include <libinfinitymm/client/clientsessionproxy.h>

#include <qinfinitymm/qtio.h>

#include "connection.h"
#include "infinotemanager.h"

#include <KDebug>

namespace Kobby
{

Connection::Connection( const QString &cname,
    const QString &chostname,
    unsigned int cport,
    QObject *parent )
    : QObject( parent )
    , name( cname )
    , hostname( chostname )
    , port( cport )
{
    infinoteManager = InfinoteManager::instance( this );
    init();

    xmppConnection->property_status().signal_changed().connect( sigc::mem_fun( this, &Connection::statusChangedCb ) );
}

#define DEL_IF_EXISTS( x ) if( x ) delete x;

Connection::~Connection()
{
    close();
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

void Connection::setName( const QString &string )
{
    name = string;
    emit( nameChanged( string ) );
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

bool Connection::isConnected() const
{
    if( xmppConnection->getStatus() == Infinity::XML_CONNECTION_OPEN )
        return true;
    
    return false;
}

void Connection::open()
{
    tcpConnection->open();
}

void Connection::close()
{
    kDebug() << "Closing connection " << getName();
    
    if( clientBrowser )
    {
        delete clientBrowser;
        clientBrowser = 0;
    }
    
    if( xmppConnection )
        xmppConnection->close();
}

Infinity::ClientBrowser &Connection::getClientBrowser() const
{
    return *clientBrowser;
}

void Connection::init()
{
    xmppConnection = &infinoteManager->createXmppConnection( hostname, port );
    tcpConnection = &xmppConnection->getTcpConnection();
    clientBrowser = new Infinity::ClientBrowser( infinoteManager->getIo(),
        *xmppConnection,
        infinoteManager->getConnectionManager() );
    clientBrowser->addPlugin( infinoteManager->getTextPlugin() );
    clientBrowser->signal_subscribe_session().connect( sigc::mem_fun( this,
        &Connection::slotSubscribeSession ) );
}

void Connection::statusChangedCb()
{
    switch( getStatus() )
    {
        case Infinity::XML_CONNECTION_OPEN:
            if ( !has_connected )
                has_connected = true;
    }

    emit( statusChanged( getStatus() ) );
}

void Connection::slotSubscribeSession( const Infinity::ClientBrowserIter &iter, Glib::RefPtr<Infinity::ClientSessionProxy> proxy )
{
    emit( sessionSubscribe( iter, proxy.operator->() ) );
}

}
