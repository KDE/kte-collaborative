#ifndef QTIO_H
#define QTIO_H

#include <libinfinitymm/common/io.h>

namespace Infinity
{

class QtIo : public Io
{

public:
    QtIo();
    
    void watch(int socket, IoEvent, IoFunction, void *user_data, Glib::Object::DestroyNotify destroy_notify);
    void* addTimeout(unsigned int timeout_msecs, TimeoutFunction notify, void *user_data, Glib::Object::DestroyNotify notify);
    void removeTimeout(void *timeout_handle);

}; // class QtIo

} // namespace Infinity

#endif
