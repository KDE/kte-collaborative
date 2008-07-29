#include <qinfinitymm/qtio.h>
#include <qinfinitymm/ioqsocketnotifier.h>

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

/* This could be made more efficient, im sure.  
What happens when socket notifiers are duplicated? 
Can setEnabled be used? */
void QtIo::watch_vfunc( int *socket, IoEvent event, IoFunction handler, void *user_data, Glib::Object::DestroyNotify destroy_notify )
{

    // Some debug info before we start destroying the watched sockets.
    qDebug() << "watch event: ";
    if( event & IO_INCOMING )
        qDebug() << "\tenable read.";
    else
        qDebug() << "\tdisable read.";
    if( event & IO_OUTGOING )
        qDebug() << "\tenable write.";
    else
        qDebug() << "\tdisable write.";
    if( event & IO_ERROR )
        qDebug() << "\tenable error.";
    else
        qDebug() << "\tdisable error.";

    if( event & IO_INCOMING )
    {
        enableNotifier( *socket, QSocketNotifier::Read, handler, user_data, destroy_notify );
    }
    else
    {
        disableNotifier( *socket, QSocketNotifier::Read );
    }

    if( event & IO_OUTGOING )
    {
        enableNotifier( *socket, QSocketNotifier::Write, handler, user_data, destroy_notify );
    }
    else
    {
        //disableNotifier( *socket, QSocketNotifier::Write );
    }

    if( event & IO_ERROR )
    {
        enableNotifier( *socket, QSocketNotifier::Exception, handler, user_data, destroy_notify );
    }
    else
    {
        disableNotifier( *socket, QSocketNotifier::Exception );
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

IoQSocketNotifier *QtIo::locateNotifier( int socket, QSocketNotifier::Type type )
{
    QList<IoQSocketNotifier*>::Iterator itr;

    for( itr = watchedSockets.begin(); itr != watchedSockets.end(); ++itr )
    {
        if( (*itr)->socket() == socket && (*itr)->type() == type )
        {
            return *itr;
        }
    }

    return 0;
}

void QtIo::enableNotifier( int socket,
    QSocketNotifier::Type type,
    IoFunction handler_func,
    void *user_data,
    Glib::Object::DestroyNotify destroy_notify
)
{
    IoQSocketNotifier *notifier;
    QAbstractEventDispatcher *eventDispatcher = QAbstractEventDispatcher::instance();

    if( (notifier = locateNotifier( socket, type )) )
    {
        qDebug() << "enabling " << IoQSocketNotifier::typeString( type ) << " notifier.";
        notifier->setEnabled( true );
        return;
    }

    qDebug() << "creating " << IoQSocketNotifier::typeString( type ) << " notifier.";
    
    notifier = new IoQSocketNotifier( socket,
        type,
        handler_func, 
        user_data,
        destroy_notify
    );

    eventDispatcher->registerSocketNotifier( notifier );
    watchedSockets.append( notifier );
    
    notifier->setEnabled( true );
}

bool QtIo::disableNotifier( int socket, QSocketNotifier::Type type )
{
    IoQSocketNotifier *notifier;

    if( (notifier = locateNotifier( socket, type )) )
    {
        qDebug() << "disabling " << IoQSocketNotifier::typeString( type ) << " notifier.";

        if( notifier->isEnabled() )
            qDebug() << "disabled.";
        else
            qDebug() << "still enabled!";
        
        return true;
    }

    return false;
}

} // namespace Infinity
