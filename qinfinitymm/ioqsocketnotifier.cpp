#include <qinfinitymm/ioqsocketnotifier.h>

#include "ioqsocketnotifier.moc"

#include <QDebug>

namespace Infinity
{

IoEvent IoQSocketNotifier::convertEventType(QSocketNotifier::Type event)
{
    IoEvent ret = IO_ERROR;
    
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
            break;
        default:
            qDebug() << "Event type (" << event << ") not found!";
    }

    return ret;
}

QSocketNotifier::Type IoQSocketNotifier::convertEventType(IoEvent event)
{
    QSocketNotifier::Type ret = QSocketNotifier::Exception;
    
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
            break;
        default:
            qDebug() << "Event type (" << event << ") not found!";
    }
    
    return ret;
}

const QString IoQSocketNotifier::typeString( QSocketNotifier::Type type )
{
    QString typeStr;
    
    switch( type )
    {
        case QSocketNotifier::Read:
            typeStr = "read";
            break;
        case QSocketNotifier::Write:
            typeStr = "write";
            break;
        case QSocketNotifier::Exception:
            typeStr = "exception";
    }

    return typeStr;
}

IoQSocketNotifier::IoQSocketNotifier( int socket,
    QSocketNotifier::Type type,
    IoFunction handler_func,
    void *user_data,
    Glib::Object::DestroyNotify destroy_notify,
    QObject *parent
)
    : QSocketNotifier( socket, type, parent )
    , socket_desc( socket )
    , handler( handler_func )
    , user_data( user_data )
    , destroy_notify( destroy_notify )
{
#if 0
    qDebug() << "creating " << IoQSocketNotifier::typeString( type );
#endif
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
#if 0
    qDebug() << IoQSocketNotifier::typeString( type() ) << " activated.";
#endif

    if( isEnabled() )
        this->handler( &this->socket_desc, IoQSocketNotifier::convertEventType( type() ), user_data );
}

} // namespace Infinity
