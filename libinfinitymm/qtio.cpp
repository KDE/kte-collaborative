#include "qtio.h"
#include "ioqsocketnotifier.h"

#include <QSocketNotifier>

namespace Infinity
{

QtIo::QtIo()
{}

QtIo::~QtIo()
{
    QList<IoQSocketNotifier*>::Iterator itr;
    
    for( itr = watchedSockets.begin(); itr != watchedSockets.end(); ++itr )
        delete *itr;
}

void QtIo::watch( int socket, IoEvent event, IoFunction handler, void *user_data, Glib::Object::DestroyNotify destroy_notify )
{
    IoQSocketNotifier *newWatch;
    
    newWatch = new IoQSocketNotifier( socket, IoQSocketNotifier::convertEventType( event ),
        handler, user_data, destroy_notify );
    
    watchedSockets.append( newWatch );
}

} // namespace Infinity
