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

#include <kobby/infinote/connection.h>

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
}

namespace Kobby
{

/**
 * @brief A manager class for Qt libinfinitymm objects.
 */
class InfinoteManager : public QObject
{
    
    Q_OBJECT
    
    public:
        InfinoteManager( QObject *parent = 0 );
        ~InfinoteManager();

        Infinity::QtIo &getIo() const;
        const QString &getJid() const;
        Infinity::ConnectionManager &getConnectionManager() const;
        const QList<Connection*> &getConnections() const;

    Q_SIGNALS:
        /**
         * @brief The Jabber ID has been changed
         */
        void jidChanged( const QString &jid );
        /**
         * @brief A connection has been added.
         */
        void connectionAdded( const Connection &connection );
        /**
         * @brief A connection has been removed.
         */
        void connectionRemoved( const Connection &connection );

    public Q_SLOTS:
        /**
         * @brief Create a new connection to host which will be managed by the InfinoteManager instance.
         * 
         * @param name Label for this connection
         * @param hostname Remote host
         * @param port Remote port
         */
        Connection &connectToHost( const QString &name, const QString &hostname, unsigned int port );
        /**
         * @brief Add connection to managed connections.
         */
        void addConnection( Connection &connection );
        void removeConnection( const Connection &connection );
        void setJid( const QString &string );

    private:
        Infinity::XmppConnection &newXmppConnection( const QString &host, unsigned int port );

        QString jid;
        Infinity::QtIo *io;
        QList<Connection*> connections;
        Infinity::ConnectionManager *connectionManager;
        gnutls_certificate_credentials_t gnutls_cred;
        Gsasl *gsasl_context;
    
}; // class InfinoteManager

} // namespace Infinity

#endif
