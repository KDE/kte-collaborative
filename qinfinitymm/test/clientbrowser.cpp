#include <libinfinitymm/init.h>
#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>
#include <libinfinitymm/common/methodmanager.h>

#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientexplorerequest.h>

#include <qinfinitymm/qtio.h>

#include <iostream>
#include <string>

#include <string.h>
#include <unistd.h>

#include <QCoreApplication>

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember)) 

void onConnectionStatusChanged( Infinity::TcpConnection* );
void myBrowserIncomingCallbackProxy( int *socket, Infinity::IoEvent event, void *user_data );

void exploreRequestCb();

class MyBrowser
{

public:
    MyBrowser( Infinity::QtIo &io, Infinity::IpAddress &address, unsigned int port, const char *jid );
    ~MyBrowser();

    void incomingCallback( Infinity::NativeSocket socket, Infinity::IoEvent event );
    void listCwdNodes( const char *param );
    void addNode( const char *param );
    void removeNode( const char *param );
    void cd( const char *param );
    void listCommands( const char *param );
    void quit( const char *param );
    void explore( const char *param );
    void onConnectionStatusChanged();

private:
    bool findNode( const char *param, Infinity::ClientBrowserIter *itr );

    std::map<std::string, void(MyBrowser::*)(const char *)> commands;
    Infinity::QtIo *io;
    Infinity::TcpConnection *tcpConnection;
    Infinity::XmppConnection *xmppConnection;
    Infinity::ConnectionManager *connectionManager;
    Infinity::ClientBrowser *clientBrowser;
    Infinity::ClientBrowserIter cwd;
    Glib::RefPtr<Infinity::ClientExploreRequest> request; // to keep request in scope
    int input_fd;
    bool has_connected;

};

MyBrowser::MyBrowser( Infinity::QtIo &io, Infinity::IpAddress &address, unsigned int port, const char *jid )
    : io( &io )
    , tcpConnection( new Infinity::TcpConnection( io, address, port ) )
    , xmppConnection( new Infinity::XmppConnection( *tcpConnection, Infinity::XMPP_CONNECTION_CLIENT, jid ) )
    , connectionManager( new Infinity::ConnectionManager() )
    , clientBrowser( new Infinity::ClientBrowser( io, *xmppConnection, *connectionManager ) )
    , input_fd( STDIN_FILENO )
    , has_connected( false )
{
    commands["ls"] = &MyBrowser::listCwdNodes;
    commands["create"] = &MyBrowser::addNode;
    commands["remove"] = &MyBrowser::removeNode;
    commands["cd"] = &MyBrowser::cd;
    commands["commands"] = &MyBrowser::listCommands;
    commands["quit"] = &MyBrowser::quit;
    commands["explore"] = &MyBrowser::explore;

    tcpConnection->property_status().signal_changed().connect( sigc::mem_fun( this, &MyBrowser::onConnectionStatusChanged ) );

    io.watch( &input_fd, Infinity::IO_INCOMING, &myBrowserIncomingCallbackProxy, this, 0 );

    tcpConnection->open();
}

MyBrowser::~MyBrowser()
{
    if( tcpConnection )
        delete tcpConnection;

    if( xmppConnection )
        delete xmppConnection;

    if( connectionManager )
        delete connectionManager;

    if( clientBrowser )
        delete clientBrowser;
}

void MyBrowser::incomingCallback( int socket, Infinity::IoEvent event )
{
    size_t param_offset = 0;
    std::string inLine;
    std::string command;
    std::map<std::string, void(MyBrowser::*)(const char *)>::iterator itr;

    while (!std::getline( std::cin, inLine ));

    command = inLine;

    param_offset = inLine.find_first_of( ' ' );
    if( param_offset != std::string::npos )
    {
        command = inLine.substr( 0, param_offset );
        param_offset++;
    }
    else
        param_offset = inLine.size();

    for( itr = commands.begin(); itr != commands.end(); ++itr )
    {
        if( command == itr->first )
        {
            CALL_MEMBER_FN( *this, itr->second )( (inLine.c_str() + param_offset) );
            return;
        }
    }
    
    std::cout << "command \'" << command << "\' not found.\n";
    listCommands( 0 );
}

void MyBrowser::listCwdNodes( const char *param )
{
    Infinity::ClientBrowserIter itr;
    bool res;

    if( !( cwd.isExplored() ) )
    {
        std::cout << "current node not explored!\n";
        return;
    }

    itr = cwd;

    std::cout << "nodes:\n";
    for( res = itr.child(); res; res = itr.next() )
    {
        std::cout << "\t" << itr.getName() << "\n";
    }

}

void MyBrowser::addNode( const char *param )
{
    if( !param )
        std::cout << "no node name specified.\n";

    std::cout << "adding \'" << param << "\'\n";

    clientBrowser->addSubdirectory( cwd, param );
}

void MyBrowser::removeNode( const char *param )
{
    Infinity::ClientBrowserIter itr;

    if( !findNode( param, &itr ) )
    {
        std::cout << "Node \'" << param << "\' not found.\n";
        return;
    }

    clientBrowser->removeNode( itr );
}

void MyBrowser::cd( const char *param )
{
    std::string parameter( param );
    Infinity::ClientBrowserIter itr;

    if( parameter.size() == 0 )
    {
        std::cout << "no location specified.\n";
    }
    else if( parameter == ".." )
    {
        if( !cwd.parent() )
        {
            std::cout << "already at root node!\n";
        }
    }
    else
    {
        if( !findNode( param, &itr ) )
            std::cout << "node \'" << param << "\' doesnt exist.\n";
        else
            cwd = itr;
    }
}

void MyBrowser::listCommands( const char *param )
{
    std::map<std::string, void(MyBrowser::*)(const char *)>::iterator itr;

    std::cout << "Available commands:\n";

    for( itr = commands.begin(); itr != commands.end(); itr++ )
    {
        std::cout << "\t" << itr->first << "\n";
    }
}

void MyBrowser::quit( const char *param )
{
    tcpConnection->close();
}

void MyBrowser::explore( const char *param )
{
    Infinity::ClientBrowserIter *itr;

    if( !(*param) )
    {
        std::cout << "no node specified, exploring current node.\n";
        request = cwd.explore();
        request->signal_finished().connect( sigc::ptr_fun( exploreRequestCb ) );
    }
    else if( !findNode( param, itr ) )
    {
        std::cout << "node " << param << " doesnt exist!\n";
    }
    else
    {
        std::cout << "exploring \'" << param << "\'.\n";
        request = cwd.explore();
        request->signal_finished().connect( sigc::ptr_fun( exploreRequestCb ) );
    }
}

void exploreRequestCb()
{
    std::cout << "explore finished.\n";
}

void MyBrowser::onConnectionStatusChanged( )
{
    switch( tcpConnection->property_status().get_value() )
    {
        case Infinity::TCP_CONNECTION_CONNECTING:
            std::cout << "connecting\n";
            break;
        case Infinity::TCP_CONNECTION_CONNECTED:
            std::cout << "connected\n";
            has_connected = true;
            clientBrowser->setRootNode( cwd );
            break;
        case Infinity::TCP_CONNECTION_CLOSED:
            if( has_connected )
                std::cout << "closed\n";
            else
                std::cout << "could not connect to server.\n";

            QCoreApplication::exit();
    }
}

bool MyBrowser::findNode( const char *param,
    Infinity::ClientBrowserIter *found
)
{
    bool res;
    Infinity::ClientBrowserIter itr;

    if( !cwd.isExplored() )
    {
        std::cout << cwd.getName() << " is not explored!\n";
        return false;
    }

    itr = cwd;
    for( res = itr.child(); res; res = itr.next() )
    {
        if( 0 == strcmp( param, itr.getName() ) )
        {
            if( found )
                *found = itr;
        }
    }

    return true;
}

void myBrowserIncomingCallbackProxy( int *socket, Infinity::IoEvent event, void *user_data )
{
    MyBrowser *browser = static_cast<MyBrowser*>(user_data);

    browser->incomingCallback( *socket, event );
}

int main( int argc, char **argv )
{
    using namespace Infinity;

    Infinity::init();

    QCoreApplication app( argc, argv );

    Infinity::QtIo io;
    Infinity::IpAddress address( "127.0.0.1" );

    MyBrowser browser( io, address, 6523, "greg@localhost" );

    app.exec();
}
