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

#ifndef KOBBY_CONNECTION_H
#define KOBBY_CONNECTION_H

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

/* We are not inheriting from XmppConnection for several reasons:
    Including libinfinitymm after a Qt include dies due to libsigc++
    Subclassing Glib::Objects isnt entirely clean due to GObject's design
*/
class Connection
    : public QObject
{
    Q_OBJECT

    public:
        Connection( InfinoteManager &manager, const QString name, Infinity::XmppConnection &conn );
        ~Connection();

        const QString &getName() const;
        Infinity::XmppConnection &getXmppConnection() const;
        Infinity::TcpConnection &getTcpConnection() const;
        Infinity::ClientBrowser &getClientBrowser() const;
        int getStatus() const;

    Q_SIGNALS:
        /**
         * The status of the TcpConnection has changed.  See Infinity::TcpConnection.
         * It is suggested to use this rather than the underlying TcpConnection signals
         * to ensure any underlying structures are initialized before the signal is emitted.
         */
        void statusChanged( int status );

    private:
        void slotStatusChanged();

        InfinoteManager *infinoteManager;
        QString name;
        Infinity::XmppConnection *xmppConnection;
        Infinity::TcpConnection *tcpConnection;
        bool has_connected;
        Infinity::ClientBrowser *clientBrowser;
};

}

#endif
