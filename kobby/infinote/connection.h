// This file may be licensed under the terms of of the
// GNU General Public License Version 2 (the "GPL").
// 
// Software distributed under the License is distributed 
// on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
// express or implied. See the GPL for the specific language 
// governing rights and limitations.
//
// You should have received a copy of the GPL along with this 
// program. If not, go to http://www.gnu.org/licenses/gpl.html
// or write to the Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#ifndef QINFINITYMM_CONNECTION_H
#define QINFINITYMM_CONNECTION_H

#include <glibmm/refptr.h>

#include <QObject>

namespace Infinity
{
    class XmppConnection;
    class TcpConnection;
    class ConnectionManager;
    class MethodManager;
    class ClientBrowser;
    class ClientBrowserIter;
    class ClientExploreRequest;
    class ClientSessionProxy;
    class QtIo;
}

namespace Kobby
{

class InfinoteManager;

/**
 * @brief Wrapper for libinfinitymm connection objects
 *
 * This class manages a connection to an infinote server.  It is
 * meant to provide convinience over controlling the underlying
 * libinfinitymm objects individually.  Using the libinfinitymm
 * objects directly can and likely will cause errors with manager
 * widgets.
 */
/*
 * We are not inheriting from XmppConnection for several reasons:
 * Including libinfinitymm after a Qt include dies due to libsigc++
 * Subclassing Glib::Objects isnt entirely clean at the current time.
 */
class Connection
    : public QObject
{

    Q_OBJECT

    public:
        /**
         * @brief Create Connection object.
         * @param manager InfinoteManager instance.
         * @param name Name of connection.  This functions only as a label and must be unique.
         * @param hostname Hostname to connect to.
         * @param port Port on Hostname to connect to.
         * @param parent Parent object
         */
        Connection( InfinoteManager &manager,
            const QString &name,
            const QString &hostname,
            unsigned int port,
            QObject *parent = 0 );
        ~Connection();

        bool operator==( const Connection &connection ) const;
        bool operator!=( const Connection &connection ) const;

        /**
         * @brief Get name for this connection.
         * @return Current name of connection.
         */
        const QString &getName() const;
        /**
         * @brief Set name for this connection.
         * @param name New name for the connection.
         *
         * The name functions purely as a label, and it must me unique.
         */
        void setName( const QString &name );
        /**
         * @brief Get the underlying XmppConnection.
         * @return XmppConnection for this Connection.
         */
        Infinity::XmppConnection &getXmppConnection() const;
        /**
         * @brief Get the underlying TcpConnection.
         * @return TcpConnection for this Connection.
         */
        Infinity::TcpConnection &getTcpConnection() const;
        /**
         * @brief Get the InfinoteManager instance this connection belongs to.
         * @return InfinoteManager instance for this Connection.
         */
        InfinoteManager &getInfinoteManager() const;
        /**
         * @brief Get the ClientBrowser for this connection.
         */
        Infinity::ClientBrowser &getClientBrowser() const;
        /**
         * @brief Status of XmppConnection
         * @return Status of type Infinity::XmlConnectionStatus.
         */
        int getStatus() const;
        /**
         * @brief Is the XML portion of this connection connected.
         * @return Is this connection fully connected.
         */
        bool isConnected() const;
        /**
         * @brief Open this connection if it is closed.
         */
        void open();
        /**
         * @brief Close this connection if it is open.
         */
        void close();

    Q_SIGNALS:
        /**
         * @brief The status of the underlying XmppConnection has changed.
         * @param status New status of this connection
         *
         * It is suggested to use this rather than the underlying TcpConnection signals
         * to ensure any underlying structures are initialized before the signal is emitted.
         */
        void statusChanged( int status );
        /**
         * @brief The name of this connection has been changed.
         * @param name New name for this connectin
         */
        void nameChanged( const QString &name );
        /**
         * @brief A subscription has been made on this connection.
         * @param node Subscribed node.
         * @param sessionProxy Proxy for the subscribed session.
         */
        void sessionSubcribe( Infinity::ClientBrowserIter &node, Infinity::ClientSessionProxy *sessionProxy );

    private:
        void init();
        void statusChangedCb();
        void slotSubscribeSession( const Infinity::ClientBrowserIter &iter, Infinity::ClientSessionProxy* );

        InfinoteManager *infinoteManager;
        QString name;
        QString hostname;
        unsigned int port;
        Infinity::XmppConnection *xmppConnection;
        Infinity::TcpConnection *tcpConnection;
        Infinity::ClientBrowser *clientBrowser;
        bool has_connected;

};

}

#endif
