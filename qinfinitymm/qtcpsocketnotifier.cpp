#include <qinfinitymm/qtcpsocketnotifier.h>

#include <QDebug>

namespace Infinity
{

QTcpSocketNotifier::QTcpSocketNotifier( int *socket,
    IoFunction handler_func,
    void *data,
    QIODevice::OpenMode mode,
    QObject *parent
)
    : socket_ptr( socket )
    , handler( handler_func )
    , user_data( data )
{
    setSocketDescriptor( *socket, mode );

    connect( this, SIGNAL(disconnected()), this, SLOT(slotError()) );
    connect( this, SIGNAL(error( QAbstractSocket::SocketError )), this, SLOT(slotError( QAbstractSocket::SocketError )) );
    connect( this, SIGNAL(readyRead()), this, SLOT(slotIncoming()) );
}

void QTcpSocketNotifier::slotIncoming()
{
    handler( socket_ptr, IO_INCOMING, user_data );
}

void QTcpSocketNotifier::slotOutgoing()
{
    handler( socket_ptr, IO_OUTGOING, user_data );
}

void QTcpSocketNotifier::slotError()
{
    handler( socket_ptr, IO_ERROR, user_data );
}

void QTcpSocketNotifier::slotError( QAbstractSocket::SocketError socketError )
{
    qDebug << errorString();
}

}
