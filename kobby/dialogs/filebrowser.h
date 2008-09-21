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

namespace Ui
{
    class NewFolderWidget;
    class FileBrowserWidget;
}

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
 * @brief An item in a FileBrowserTreeWidget which represents a node on the infinote server.
 */
class FileBrowserWidgetItem
    :    public QObject
    , public QTreeWidgetItem
{

    public:
        /**
         * @brief Type of node this item can represent.
         */
        enum ItemType { Folder = 1001, Note = 1002 };

        /**
         * @brief Create an item representing a node on the infinote server without calling getName on the iterator.
         * @param name Name of this node.
         * @param iter Iterator pointing to the node.
         * @param type Type of node.
         * @param parent Parent widget.
         */
        FileBrowserWidgetItem( QString name, const Infinity::ClientBrowserIter &iter, int type, QTreeWidget *parent = 0 );
        /**
         * @brief Create an item represented by a node on the infinote server.
         * @param iter Iterator pointing to the node.
         * @param type Type of node.
         * @param parent Parent widget.
         */
        FileBrowserWidgetItem( const Infinity::ClientBrowserIter &iter, int type, QTreeWidget *parent );

        /**
         * @brief Get an iterator pointing to node this item represents.
         * @return Iterator pointing to the underlying node.
         */
        Infinity::ClientBrowserIter &getNode() const;

    private:
        void setupUi();

        Infinity::ClientBrowserIter *node;

};

/**
 * @brief A note item in a FileBrowserWidget.
 */
class FileBrowserWidgetNoteItem
    : public FileBrowserWidgetItem
{

    public:
        /**
         * @brief Create a note item representing a note on the infinote server.
         * @param iter Iterater pointing to note node.
         * @param parent Parent tree widget.
         */
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
        /**
         * @brief Create folder item representing a folder on the infinote server.
         * @param iter Iterator pointer to folder node.
         * @param parent Parent tree widget.
         */
        FileBrowserWidgetFolderItem( Infinity::ClientBrowserIter &iter, QTreeWidget *parent = 0 );
        /**
         * @brief Create folder item using the specified name instead of calling getName() on ClientBrowserIter.
         * @param name Name of the folder.
         * @param iter Iterator ponting to folder node.
         * @param parent Parent tree widget.
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
 * @brief Tree widget of nodes accessible over a connection.
 *
 * This clas provides a browser of nodes on the infinote server.  See FileBrowserWidget
 * for controlling these nodes.
 */
class FileBrowserTreeWidget
    : public QTreeWidget
{

    Q_OBJECT

    public:
        FileBrowserTreeWidget( QWidget *parent = 0 );
        FileBrowserTreeWidget( Connection &connection, QWidget *parent = 0 );
        ~FileBrowserTreeWidget();

        QList<FileBrowserWidgetItem*> getSelectedNodes();
        InfinoteManager *getInfinoteManager() const;
        Connection *getConnection() const;

    public Q_SLOTS:
        /**
         * @brief Unset the current connection, leaving the browser disabled.
         */
        void unsetConnection();
        /**
         * @brief Set the current Connection, enabling the browser.
         *
         * Use this to set which connection the file browser is using.  If
         * connection is NULL then unsetConnection() will be called and the
         * widget will be disabled.
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
        void nodeAddedCb( Infinity::ClientBrowserIter node );
        void nodeRemovedCb( Infinity::ClientBrowserIter node );
        FileBrowserWidgetItem *findNodeItem( Infinity::ClientBrowserIter &iter );
        FileBrowserWidgetItem *findNodeInList( Infinity::ClientBrowserIter &iter, QList<QTreeWidgetItem*> items );

        InfinoteManager *infinoteManager;
        Infinity::ClientBrowser *clientBrowser;
        Connection *connection;
        Infinity::ClientBrowserIter *rootNode;
        Glib::RefPtr<Infinity::ClientExploreRequest> *exploreRequest;

};

class NewFolderDialog
    : public KDialog
{

    Q_OBJECT

    public:
        NewFolderDialog( Infinity::ClientBrowserIter &parentIter, QWidget *parent = 0 );

    Q_SIGNALS:
        void create( const QString &name, Infinity::ClientBrowserIter &parentIter );

    private Q_SLOTS:
        void slotCreate();

    private:
        void setupUi();
        void setupActions();

        Ui::NewFolderWidget *ui;
        Infinity::ClientBrowserIter *parentIter;

};

/**
 * @brief A widget to manage the nodes on an infinote server.
 */
class FileBrowserWidget
    : public QWidget
{

    Q_OBJECT

    public:
        FileBrowserWidget( Connection &connection, QWidget *parent = 0 );
        FileBrowserWidget( QWidget *parent = 0 );

        FileBrowserTreeWidget &getTreeWidget() const;

    public Q_SLOTS:
        void addFolder( const QString &name, Infinity::ClientBrowserIter &parentNode );

    private Q_SLOTS:
        void slotNodeSelectionChanged();
        void slotNewFolderDialog();
        void slotRemoveNodes();

    private:
        void setupUi();
        void setupActions();

        Ui::FileBrowserWidget *ui;
};

/**
 * @brief Dialog containing FileBrowserWidget
 */
class FileBrowserDialog
    : public KDialog
{

    public:
        FileBrowserDialog( Connection &connection, QWidget *parent = 0 );
        ~FileBrowserDialog();

    private:
        void setupUi();

        FileBrowserWidget *fileBrowserWidget;
};

}

#endif
