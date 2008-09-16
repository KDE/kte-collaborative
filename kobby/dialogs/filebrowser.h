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

#include <KDialog>

#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

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

typedef struct _GError GError;

class InfinoteManager;
class Connection;

/**
 * @brief An item of a FileBrowserWidget tree
 */
class FileBrowserWidgetItem
    :    public QObject
    , public QTreeWidgetItem
{

    public:
        enum ItemType { Folder = 1001, Note = 1002 };

        FileBrowserWidgetItem( QString name, const Infinity::ClientBrowserIter &iter, int type, QTreeWidget *parent = 0 );
        FileBrowserWidgetItem( const Infinity::ClientBrowserIter &iter, int type, QTreeWidget *parent );

        Infinity::ClientBrowserIter &getNode() const;

    protected:
        Infinity::ClientBrowserIter *node;

    private:
        void setupUi();

};

/**
 * @brief A note item in a FileBrowserWidget.
 */
class FileBrowserWidgetNoteItem
    : public FileBrowserWidgetItem
{

    public:
        FileBrowserWidgetNoteItem( Infinity::ClientBrowserIter &iter, QTreeWidget *parent = 0 );

};

/**
 * @brief A folder item in a FileBrowserWidget.
 */
class FileBrowserWidgetFolderItem
    : public FileBrowserWidgetItem
{

    Q_OBJECT

    public:
        FileBrowserWidgetFolderItem( Infinity::ClientBrowserIter &iter, QTreeWidget *parent = 0 );
        /**
         * @brief Create using the specified name instead of calling getName() on ClientBrowserIter
         *
         * Use this to crete a folder item without calling getName() on the ClientBrowserIter.
         */
        FileBrowserWidgetFolderItem( QString name, Infinity::ClientBrowserIter &iter, QTreeWidget *parent = 0 );
        ~FileBrowserWidgetFolderItem();

    public Q_SLOTS:
        /**
         * @brief Populate this folder with child nodes.
         */
        void populate( bool expand_when_finished = true );

    private:
        void setupUi();
        void exploreFinishedCb();
        void exploreFailedCb( GError * );

        Glib::RefPtr<Infinity::ClientExploreRequest> *exploreRequest;
        bool is_populated;

};

/**
 * @brief FileBrowser for a connection.
 */
class FileBrowserWidget
    : public QTreeWidget
{

    Q_OBJECT

    public:
        FileBrowserWidget( QWidget *parent = 0 );
        FileBrowserWidget( const Connection &connection, QWidget *parent = 0 );
        ~FileBrowserWidget();

    public Q_SLOTS:
        /**
         * @brief Unset the current connection, leaving the browser disabled.
         */
        void unsetConnection();
        /**
         * @brief Set the current Connection, enabling the browser.
         */
        void setConnection( Connection *connection );

    private Q_SLOTS:
        void slotItemExpanded( QTreeWidgetItem *item );
        void slotConnectionStatusChanged( int );

    private:
        void setupUi();
        void setupActions();
        void setupConnectionActions();
        void createRootNodes();

        InfinoteManager *infinoteManager;
        Infinity::ClientBrowser *clientBrowser;
        const Connection *connection;
        Infinity::ClientBrowserIter *rootNode;
        Glib::RefPtr<Infinity::ClientExploreRequest> *exploreRequest;

};

/**
 * @brief Dialog containing FileBrowserWidget
 */
class FileBrowserDialog
    : public KDialog
{

    public:
        FileBrowserDialog( const Connection &connection, QWidget *parent = 0 );
        ~FileBrowserDialog();

    private:
        void setupUi();

        FileBrowserWidget *fileBrowserWidget;
};

}

#endif
