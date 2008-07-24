#ifndef IOQSOCKETNOTIFIER_H
#define IOQSOCKETNOTIFIER_H

#include <libinfinitymm/common/io.h>
#include <glibmm/object.h>

#include <QSocketNotifier>

namespace Infinity
{

class IoQSocketNotifier : public QSocketNotifier
{
    Q_OBJECT

public:
    static QSocketNotifier::Type convertEventType(IoEvent event);
    static IoEvent convertEventType(QSocketNotifier::Type event);

public:
    IoQSocketNotifier( int socket, QSocketNotifier::Type type, IoFunction handler_func,
        void *user_data, Glib::Object::DestroyNotify destroy_notifiy, QObject *parent = 0 );
    ~IoQSocketNotifier();
    
    void *getUserData();

private Q_SLOTS:
    void slotActivated();

private:
    IoFunction handler;
    void *user_data;
    Glib::Object::DestroyNotify destroy_notify;
    int socket_fd; // This is redundant, but inf_io_watch wants a socket pointer.

}; // class SocketNotifier

} // namespace Infinity

#endif

