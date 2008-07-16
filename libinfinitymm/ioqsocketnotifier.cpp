#include "ioqsocketnotifier.h"

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
}

IoQSocketNotifier::IoQSocketNotifier( int socket,
    QSocketNotifier::Type type,
    void *user_data,
    IoFunction handler_func,
    Glib::Object::DestroyNotify destroy_notify,
    QObject *parent
)
    : QSocketNotifier( socket, type, parent ),
        socket_fd( socket ),
        user_data( user_data ),
        handler( handler_func ),
        destroy_notify( destroy_notify )
{}

IoQSocketNotifier::~IoQSocketNotifier()
{
    this->destroy_notify( this->user_data );
}

void *IoQSocketNotifier::getUserData()
{
    return user_data;
}

void IoQSocketNotifier::slotActivated()
{
    this->handler( &socket_fd, IoQSocketNotifier::convertEventType( type() ), user_data );
}

} // namespace Infinity
