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
    class QtIo;
}

namespace Kobby
{

class InfinoteManager;

/**
 * @brief Wrapper for libinfinitymm connection objects
 *
 * We are not inheriting from XmppConnection for several reasons:
 * Including libinfinitymm after a Qt include dies due to libsigc++
 * Subclassing Glib::Objects isnt entirely clean at the current time.
 */
class Connection
    : public QObject
{

    Q_OBJECT

    public:
        Connection( InfinoteManager &manager,
            const QString &name,
            const QString &jid,
            const QString &hostname,
            unsigned int port,
            QObject *parent = 0 );
        ~Connection();

        bool operator==( const Connection &connection ) const;
        bool operator!=( const Connection &connection ) const;

        const QString &getName() const;
        void setName( const QString &name );
        Infinity::XmppConnection &getXmppConnection() const;
        Infinity::TcpConnection &getTcpConnection() const;
        InfinoteManager &getInfinoteManager() const;
        int getStatus() const;
        bool isConnected() const;
        void open();
        void close();
        Infinity::ClientBrowser &getClientBrowser() const;

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

    private:
        void init();
        void statusChangedCb();

        InfinoteManager *infinoteManager;
        QString name;
        QString jid;
        QString hostname;
        unsigned int port;
        Infinity::XmppConnection *xmppConnection;
        Infinity::TcpConnection *tcpConnection;
        Infinity::ClientBrowser *clientBrowser;
        bool has_connected;

};

}

#endif
