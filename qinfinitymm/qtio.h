#ifndef QINFINITYMM_QTIO_H
#define QINFINITYMM_QTIO_H

#include <qinfinitymm/ioqsocketnotifier.h>

#include <glibmm/object.h>
#include <libinfinitymm/common/io.h>

#include <QList>

namespace Infinity
{

class QtIo
    : public Glib::Object
    , public Io
    , public QObject
{

public:
    QtIo( QObject *parent = 0 );
    ~QtIo();

    void watch_vfunc(int *socket, IoEvent, IoFunction, void *user_data, Glib::Object::DestroyNotify destroy_notify);
    void *addTimeout_vfunc(unsigned int timeout_msecs, TimeoutFunction notify, void *user_data, Glib::Object::DestroyNotify notify);
    void removeTimeout_vfunc(void *timeout_handle);

private:
    void createNotifier( int socket,
        QSocketNotifier::Type type,
        IoFunction handler_func,
        void *user_data,
        Glib::Object::DestroyNotify destroy_notify
    );
    void removeNotifier( QList<IoQSocketNotifier*>::Iterator itr );

    QList<IoQSocketNotifier*>  watchedSockets;

}; // class QtIo

} // namespace Infinity

#endif
