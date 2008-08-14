#include <qinfinitymm/qtio.h>
#include <qinfinitymm/ioqsocketnotifier.h>

#include <QSocketNotifier>
#include <QAbstractEventDispatcher>

#include <QDebug>

namespace Infinity
{

QtIo::QtIo( QObject *parent )
    : Glib::ObjectBase("QtIo")
    , Io()
    , QObject( parent )
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
    QList<IoQSocketNotifier*>::Iterator itr;
    bool set;
    
    do
    {
        set = false;

        for( itr = watchedSockets.begin(); itr != watchedSockets.end(); ++itr )
        {
            if( (*itr)->socket() == *socket )
            {
    #if 0
                qDebug() << "deleting " << IoQSocketNotifier::typeString( (*itr)->type() );
    #endif
                removeNotifier( itr );
                set = true;
                break;
            }
        }
    } while( set );

    if( event & IO_INCOMING )
    {
        createNotifier( *socket, QSocketNotifier::Read, handler, user_data, destroy_notify );
    }

    if( event & IO_OUTGOING )
    {
        createNotifier( *socket, QSocketNotifier::Write, handler, user_data, destroy_notify );
    }

    if( event & IO_ERROR )
    {
        createNotifier( *socket, QSocketNotifier::Exception, handler, user_data, destroy_notify );
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

void QtIo::createNotifier( int socket,
    QSocketNotifier::Type type,
    IoFunction handler_func,
    void *user_data,
    Glib::Object::DestroyNotify destroy_notify
)
{
    QAbstractEventDispatcher *eventDispatcher = QAbstractEventDispatcher::instance();
    IoQSocketNotifier *notifier;

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

void QtIo::removeNotifier( QList<IoQSocketNotifier*>::Iterator itr )
{
    QAbstractEventDispatcher *eventDispatcher = QAbstractEventDispatcher::instance();

    (*itr)->setEnabled( false );
    eventDispatcher->unregisterSocketNotifier( *itr);
    watchedSockets.erase( itr );
}

} // namespace Infinity
