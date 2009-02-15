#ifndef KOBBY_CONNECTION_H
#define KOBBY_CONNECTION_H

#include <QObject>

#include <glib/gerror.h>

class QHostInfo;

namespace QInfinity
{
    class TcpConnection;
    class XmppConnection;
}

namespace Kobby
{

/**
 * @brief Ties connection/creation monitoring to simple interface.
 */
class Connection
    : public QObject
{
    Q_OBJECT

    public:
        Connection( const QString &hostname,
            unsigned int port,
            QObject *parent = 0 );

        void open();

    Q_SIGNALS:
        void connecting();
        void connected();
        void disconnecting();
        void disconnected();
        void error( QString message );

    private Q_SLOTS:
        void slotHostnameLookedUp( const QHostInfo &hostInfo );
        void slotStatusChanged();
        void slotError( const GError *err );

    private:
        QString m_hostname;
        unsigned int m_port;
        QInfinity::TcpConnection *m_tcpConnection;
        QInfinity::XmppConnection *m_xmppConnection;

};

}

#endif

