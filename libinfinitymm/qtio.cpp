#include "qtio.h"
#include "ioqsocketnotifier.h"

#include <QSocketNotifier>

namespace Infinity
{

QtIo::QtIo()
{}

void QtIo::watch( int socket, IoEvent event, IoFunction, void *user_data, Glib::Object::DestroyNotify destroy_notify )
{
}

} // namespace Infinity
