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

#include <KDialog>

#include <QStringList>
#include <QTreeWidget>

#include <glibmm/refptr.h>

namespace Infinity
{
    class XmppConnection;
    class ClientBrowser;
    class ClientBrowserIter;
    class ConnectionManager;
    class ClientExploreRequest;
}

namespace Kobby
{

class FileBrowserWidgetItem
    : public QObject
    , public QTreeWidgetItem
{

    Q_OBJECT

    public:
        enum ItemType { Folder = 1001, File = 1002 };

        FileBrowserWidgetItem( QStringList &strings, int type, Infinity::ClientBrowserIter &iter, QTreeWidget *parent = 0 );
        FileBrowserWidgetItem( const Infinity::ClientBrowserIter &iter, int type, QTreeWidget *parent );
        ~FileBrowserWidgetItem();

    public Q_SLOTS:
        void populateChildren();

    private:
        void setItemIcon();
        void exploreFinishedCb();

        Infinity::ClientBrowserIter *node;
        Glib::RefPtr<Infinity::ClientExploreRequest> exploreRequest;

};

class FileBrowserDialog
    : public KDialog
{

    public:
        FileBrowserDialog( InfinoteManager &manager, Connection &conn, QWidget *parent = 0 );
        ~FileBrowserDialog();

    private:
        void addRootNodes();
        void exploreFinishedCb();

        QTreeWidget *nodeTreeWidget;
        InfinoteManager *infinoteManager;
        Connection *connection;
        Infinity::ClientBrowserIter *rootNode;
        Glib::RefPtr<Infinity::ClientExploreRequest> exploreRequest;

};

}

#endif
