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

#ifndef KOBBY_FILEBROWSER_H
#define KOBBY_FILEBROWSER_H

#include "infinotemanager.h"

#include <QTreeWidget>

namespace Infinity
{

class XmppConnection;
class ClientBrowser;
class ConnectionManager;

}

namespace Kobby
{

class FileBrowserWidgetItem
    : public QTreeWidgetItem
{

    public:
        enum ItemType { Folder = 1, File = 2 };
        FileBrowserWidgetItem( QStringList &strings, int type );

}

class FileBrowserWidget
    : public QTreeWidget
{

    public:
        FileBrowserWidget( InfinoteManager &manager, Infinity::XmppConnection &conn, QWidget *parent = 0 );

    private:
        InfinoteManager *infinoteManager;
        Infinity::XmppConnection *xmppConnection;
        Infinity::ConnectionManager *connectionManager;
        Infinity::ClientBrowser *clientBrowser;

};

}

#endif
