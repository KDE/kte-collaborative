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

#ifndef KOBBY_INFINOTEMANAGER_H
#define KOBBY_INFINOTEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>

typedef struct Gsasl Gsasl;
typedef struct gnutls_certificate_credentials_st* gnutls_certificate_credentials_t;

namespace Infinity
{
    class StandaloneIo;
    class XmppConnection;
    class QtIo;
    class ConnectionManager;
    class ClientNotePlugin;
}

namespace Kobby
{
    
    class Connection;

/**
 * @brief A manager class for libinfinitymm objects.
 *
 * This class contains various helper functions to make working with libinfinitymm and
 * qinfinitymm easier.  It also contains instances for libinfinitymm which are
 * needed in several other classes.
 */
class InfinoteManager : public QObject
{
    
    Q_OBJECT
    
    public:
        /**
         * @brief Retrieve an instance of the manager.
         * @param parent Parent QObject
         *
         * Use this method to obtain an instance of the Infinote Manager.
         */
        static InfinoteManager *instance( QObject *parent = 0 );

        ~InfinoteManager();

        /**
         * @brief Get QtIo instance
         * @return Current QtIo Instance
         */
        Infinity::QtIo &getIo() const;
        /**
         * @brief Get ConnectionManager for connections
         * @return Current ConnectionManager
         */
        Infinity::ConnectionManager &getConnectionManager() const;
        /**
         * @brief Get current connections
         * @return List of current connections
         */
        QList<Connection*> &getConnections();
        /**
         * @brief Get the current text plugin.
         * @return A ClientNotePlugin named InfText
         */
        Infinity::ClientNotePlugin &getTextPlugin() const;
        /**
         * @brief Create an XmppConnection and TcpConnection, without connecting
         * @param hostname Hostname to connect to
         * @param port Port to connect to on Hostname
         * @return Unconnected XmppConnection
         */
        Infinity::XmppConnection &createXmppConnection( const QString &hostname,
            unsigned int port );
        /**
         * @brief Get the hostname of the local computer.
         * @return Local hostname
         *
         * This will be modified in the future to read from KDE settings.
         */
        const QString &getLocalHostname() const;

    Q_SIGNALS:
        /**
         * @brief A connection has been added.
         */
        void connectionAdded( Connection &connection );
        /**
         * @brief A connection has been removed.
         */
        void connectionRemoved( Connection &connection );

    public Q_SLOTS:
        /**
         * @brief Create a new connection to host which will be managed by the InfinoteManager instance.
         * 
         * @param name Label for this connection
         * @param hostname Remote host
         * @param port Remote port
         */
        Connection &connectToHost( const QString &name,
            const QString &hostname,
            unsigned int port );
        /**
         * @brief Add connection to managed connections.
         * @param connection Connection to add.
         */
        void addConnection( Connection &connection );
        /**
         * @brief Remove a connection from the managed connections.
         * @param connection Connection to remove.
         */
        void removeConnection( Connection &connection );

    protected:
        /**
         * @brief Create instance of an InfinoteManager class.
         * @param parent Parent QObject.
         */
        InfinoteManager( QObject *parent = 0 );

    private:
        static InfinoteManager *_instance;
        
        Infinity::QtIo *io;
        QList<Connection*> connections;
        Infinity::ConnectionManager *connectionManager;
        Infinity::ClientNotePlugin *textPlugin;
        QString localHostname;
        gnutls_certificate_credentials_t gnutls_cred;
        Gsasl *gsasl_context;
    
}; // class InfinoteManager

} // namespace Infinity

#endif
