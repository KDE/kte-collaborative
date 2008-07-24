#include "qtio.h"
#include "ioqsocketnotifier.h"

#include <QSocketNotifier>

namespace Infinity
{

QtIo::QtIo()
    : Glib::ObjectBase("QtIo")
    , Io()
{}

QtIo::~QtIo()
{
    QList<IoQSocketNotifier*>::Iterator itr;
    
    for( itr = watchedSockets.begin(); itr != watchedSockets.end(); ++itr )
        delete *itr;
}

void QtIo::watch_vfunc( int socket, IoEvent event, IoFunction handler, void *user_data, Glib::Object::DestroyNotify destroy_notify )
{
    IoQSocketNotifier *newWatch;
    
    newWatch = new IoQSocketNotifier( socket, IoQSocketNotifier::convertEventType( event ),
        handler, user_data, destroy_notify );
    
    watchedSockets.append( newWatch );
}

void *QtIo::addTimeout_vfunc( unsigned int timeout_msecs, TimeoutFunction notify, void *user_data, Glib::Object::DestroyNotify destroy_notify )
{
}

void QtIo::removeTimeout_vfunc( void *timeout )
{
}

} // namespace Infinity
