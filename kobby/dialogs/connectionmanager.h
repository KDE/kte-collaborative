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

#ifndef KOBBY_CONNECTIONMANAGER_H
#define KOBBY_CONNECTIONMANAGER_H

#include <KDialog>

#include <QList>
#include <QListWidget>
#include <QWidget>

namespace Infinity
{
    class TcpConnection;
    class XmppConnection;
}

namespace Kobby
{

class InfinoteManager;
class Connection;
class AddConnectionDialog;

class ConnectionListWidgetItem
    : public QObject
    , public QListWidgetItem
{

    Q_OBJECT

    public:
        ConnectionListWidgetItem( const Connection &conn, QListWidget *parent = 0 );

        void setDisplay();
        const Connection &getConnection();

    public Q_SLOTS:
        void slotStatusChanged( int status );

    private:
        const Connection *connection;
        Infinity::TcpConnection *tcpConnection;
        bool has_connected;

};

/**
 * @brief A QListWidget of the current connections.
 */
class ConnectionListWidget
    : public QListWidget
{

    Q_OBJECT

    public:
        ConnectionListWidget( InfinoteManager &infinoteManager, QWidget *parent = 0 );

    public Q_SLOTS:
        /**
         * @brief Add a connection to the displayed list.
         */
        void addConnection( const Connection &connection );
        /**
         * @brief Remove a connection from the displayed list.
         */
        void removeConnection( const Connection &connection );
        /**
         * @brief Add a list of connections to the displayed list.
         */
        void addConnections( const QList<Connection*> &connections );

    private:
        void setupActions();

        InfinoteManager *infinoteManager;

};

/**
 * @brief Widget containing a ConnectionList and add / remove buttons.
 */
class ConnectionManagerWidget
    : public QWidget
{

    Q_OBJECT

    public:
        ConnectionManagerWidget( InfinoteManager &infinoteManager, QWidget *parent = 0 );

    private Q_SLOTS:
        void slotAddConnection();
        void slotAddConnectionFinished();
        void slotRemoveConnection();
        void slotItemSelectionChanged();

    private:
        void setupUi();
        void setupActions();

        InfinoteManager *infinoteManager;
        ConnectionListWidget *connectionListWidget;
        AddConnectionDialog *addConnectionDialog;
        QPushButton *addButton;
        QPushButton *removeButton;

};

class ConnectionManagerDialog
    : public KDialog
{

    public:
        ConnectionManagerDialog( InfinoteManager &infinoteManager, QWidget *parent = 0 );

    private:
        void setupUi();

        InfinoteManager *infinoteManager;
        ConnectionManagerWidget *connectionManagerWidget;

};

}

#endif
