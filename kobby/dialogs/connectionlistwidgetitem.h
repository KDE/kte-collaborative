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

#ifndef KOBBY_CONNECTIONLISTWIDGETITEM_H
#define KOBBY_CONNECTIONLISTWIDGETITEM_H

#include <kobby/infinote/connection.h>

#include <QObject>
#include <QListWidgetItem>

namespace Infinity
{
    class TcpConnection;
    class XmppConnection;
}

namespace Kobby
{

class ConnectionListWidgetItem
    : public QObject
    , public QListWidgetItem
{

    Q_OBJECT

    public:
        ConnectionListWidgetItem( Kobby::Connection &conn, QListWidget *parent = 0 );

        void setDisplay();
        Kobby::Connection &getConnection();

    public Q_SLOTS:
        void slotStatusChanged( int status );

    private:
        Kobby::Connection *connection;
        Infinity::TcpConnection *tcpConnection;
        bool has_connected;

};

}

#endif

