#ifndef QINFINITYMM_TCPSOCKETNOTIFIER_H
#define QINFINITYMM_TCPSOCKETNOTIFIER_H

#include <libinfinitymm/common/io.h>

#include <QTcpSocket>

namespace Infinity
{

class QTcpSocketNotifier
    : public QTcpSocket
{

    public:
        QTcpSocketNotifier( int *socket,
            IoFunction handler,
            void *user_data,
            QIODevice::OpenMode mode = QIODevice::ReadWrite,
            QObject *parent = 0
        );

    private Q_SLOTS:
        void slotIncoming();
        void slotOutgoing();
        void slotError();

    private:
        int *socket_ptr; // libinfinity accepts a socket pointer for handler function
        IoFunction handler;
        void *user_data;

}; // class TcpSocketNotifier

} // namespace Infinity

#endif
