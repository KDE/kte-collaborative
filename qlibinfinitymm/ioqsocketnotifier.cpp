#include <qlibinfinitymm/ioqsocketnotifier.h>

#include "ioqsocketnotifier.moc"

#include <QDebug>

namespace Infinity
{

IoEvent IoQSocketNotifier::convertEventType(QSocketNotifier::Type event)
{
    IoEvent ret;
    
    switch (event)
    {
        case QSocketNotifier::Read:
            ret = IO_INCOMING;
            break;
        case QSocketNotifier::Write:
            ret = IO_OUTGOING;
            break;
        case QSocketNotifier::Exception:
            ret = IO_ERROR;
    }
    
    return ret;
}

QSocketNotifier::Type IoQSocketNotifier::convertEventType(IoEvent event)
{
    QSocketNotifier::Type ret;
    
    switch (event)
    {
        case IO_INCOMING:
            ret = QSocketNotifier::Read;
            break;
        case IO_OUTGOING:
            ret = QSocketNotifier::Write;
            break;
        case IO_ERROR:
            ret = QSocketNotifier::Exception;
    }
    
    return ret;
}

IoQSocketNotifier::IoQSocketNotifier( int socket,
    QSocketNotifier::Type type,
    IoFunction handler_func,
    void *user_data,
    Glib::Object::DestroyNotify destroy_notify,
    QObject *parent
)
    : QSocketNotifier( socket, type, parent )
    , socket_fd( socket )
    , handler( handler_func )
    , user_data( user_data )
    , destroy_notify( destroy_notify )
{
    connect( this, SIGNAL(activated( int )), this, SLOT(slotActivated( int )) );
}

IoQSocketNotifier::~IoQSocketNotifier()
{
    this->destroy_notify( this->user_data );
}

void *IoQSocketNotifier::getUserData()
{
    return user_data;
}

void IoQSocketNotifier::slotActivated( int socket )
{
    this->handler( &socket_fd, IoQSocketNotifier::convertEventType( type() ), user_data );
}

} // namespace Infinity
