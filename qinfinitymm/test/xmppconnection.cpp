#include <libinfinitymm/init.h>
#include <libinfinitymm/common/xmppconnection.h>

#include <qinfinitymm/qtio.h>
#include <qinfinitymm/ioqsocketnotifier.h>

#include <QCoreApplication>

#include <iostream>

void statusChanged(Infinity::TcpConnection *conn)
{
    std::cout << "staus changed\n";

    switch (conn->property_status())
    {
        case Infinity::TCP_CONNECTION_CONNECTED:
            std::cout << "\tconnected\n";
            break;
        case Infinity::TCP_CONNECTION_CONNECTING:
            std::cout << "\tconnecting\n";
            break;
        case Infinity::TCP_CONNECTION_CLOSED:
            std::cout << "\tclosed\n";
            break;
        default:
            std::cout << "\tother\n";
    }
}

int main(int argc, char **argv)
{
    Infinity::init();
    
    QCoreApplication app(argc, argv);
    
    Infinity::QtIo io( &app );
    Infinity::IpAddress address;
    
    Infinity::TcpConnection conn(io, address, 5223);
    
    conn.property_status().signal_changed().connect( sigc::bind( sigc::ptr_fun( &statusChanged ), &conn ) );
    
    if( !conn.open() )
        return -1;
    
    app.exec();
}
