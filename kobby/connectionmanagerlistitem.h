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

#ifndef KOBBY_CONNECTIONMANAGERLISTITEM_H
#define KOBBY_CONNECTIONMANAGERLISTITEM_H

#include <QString>
#include <QListWidgetItem>

namespace Infinity
{
    class XmppConnection;
} // namespace Infinity

namespace Kobby
{

class ConnectionManagerListItem : public QListWidgetItem
{
    
    public:
        ConnectionManagerListItem( Infinity::XmppConnection &conn, 
            const QString &hostname, QListWidget *parent = 0 );
        ~ConnectionManagerListItem();
    
    private:
        void onConnectionStatusChanged();
        void setStatusDisplay();
        
        Infinity::XmppConnection *connection;
        QString hostname;
        bool has_connected;
    
}; // class ConnectionManagerListItem

} // namespace Kobby

#endif
