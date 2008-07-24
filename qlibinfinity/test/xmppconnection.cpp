#include <libinfinitymm/init.h>
#include <libinfinitymm/common/xmppconnection.h>

#include <qtio.h>
#include <ioqsocketnotifier.h>

#include <QCoreApplication>

int main(int argc, char **argv)
{
    Infinity::init();
    
    QCoreApplication app(argc, argv);
    
    Infinity::QtIo io;
    Infinity::IpAddress address;

    Infinity::TcpConnection conn(io, address, 5223);

    conn.open();
}
