#include <qlibinfinitymm/qtio.h>
#include <qlibinfinitymm/ioqsocketnotifier.h>

#include <QSocketNotifier>
#include <QAbstractEventDispatcher>

#include <QDebug>

namespace Infinity
{

QtIo::QtIo()
    : Glib::ObjectBase("QtIo")
    , Io()
{
}

QtIo::~QtIo()
{
    QList<IoQSocketNotifier*>::Iterator itr;
    
    for( itr = watchedSockets.begin(); itr != watchedSockets.end(); ++itr )
        delete *itr;
}

void QtIo::watch_vfunc( int *socket, IoEvent event, IoFunction handler, void *user_data, Glib::Object::DestroyNotify destroy_notify )
{
    QAbstractEventDispatcher *eventDispatcher;
    IoQSocketNotifier *newWatch;
    
    eventDispatcher = QAbstractEventDispatcher::instance();
    
    if( event & Infinity::IO_INCOMING ) {
        qDebug() << "Creating incoming socket notifier.";
        
        newWatch = new IoQSocketNotifier( *socket,
            IoQSocketNotifier::convertEventType( Infinity::IO_INCOMING ),
            handler, 
            user_data,
            destroy_notify
        );
        
        eventDispatcher->registerSocketNotifier( newWatch );
        watchedSockets.append( newWatch );
        newWatch->setEnabled( true );
    }

    if( event & Infinity::IO_OUTGOING ) {
        qDebug() << "Creating outgoing socket notifier.";
        
        newWatch = new IoQSocketNotifier( *socket,
            IoQSocketNotifier::convertEventType( Infinity::IO_OUTGOING ),
            handler,
            user_data,
            destroy_notify
        );
        
        eventDispatcher->registerSocketNotifier( newWatch );
        watchedSockets.append( newWatch );
        newWatch->setEnabled( true );
    }
}

void *QtIo::addTimeout_vfunc( unsigned int timeout_msecs, TimeoutFunction notify, void *user_data, Glib::Object::DestroyNotify destroy_notify )
{
    Q_UNUSED( timeout_msecs )
    Q_UNUSED( notify )
    Q_UNUSED( user_data )
    Q_UNUSED( destroy_notify )
    
    return 0;
}

void QtIo::removeTimeout_vfunc( void *timeout )
{
    Q_UNUSED( timeout )
}

} // namespace Infinity
