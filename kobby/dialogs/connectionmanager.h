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

class QLineEdit;

namespace Infinity
{
    class TcpConnection;
    class XmppConnection;
}

namespace Ui
{
    class ConnectionEditor;
    class AddConnectionDialog;
}

namespace Kobby
{

class InfinoteManager;
class Connection;
class AddConnectionDialog;

/**
 * @brief A connection item in the ConnectionListWidget.
 */
class ConnectionListWidgetItem
    : public QObject
    , public QListWidgetItem
{

    Q_OBJECT

    public:
        ConnectionListWidgetItem( Connection &conn, QListWidget *parent = 0 );

        /**
         * @brief Redraw the displayed text.
         */
        void setDisplay();
        /**
         * @brief Get the connection represented by this item.
         */
        Connection &getConnection();

    public Q_SLOTS:
        void slotStatusChanged( int status );

    private:
        Connection *connection;
        Infinity::TcpConnection *tcpConnection;
        bool has_connected;

};

/**
 * @brief A QListWidget of the current connections.
 *
 * A list widget displaying the current connections.  See ConnectionManagerWidget
 * for a widget to manage these connections.
 */
class ConnectionListWidget
    : public QListWidget
{

    Q_OBJECT

    public:
        ConnectionListWidget( InfinoteManager &infinoteManager, QWidget *parent = 0 );

    Q_SIGNALS:
        /**
         * @brief The selected connection has been changed.
         */
        void connectionSelectionChanged( Connection *connection );

    public Q_SLOTS:
        /**
         * @brief Add a connection to the displayed list.
         */
        void addConnection( Connection &connection );
        /**
         * @brief Remove a connection from the displayed list.
         */
        void removeConnection( Connection &connection );
        /**
         * @brief Add a list of connections to the displayed list.
         */
        void addConnections( QList<Connection*> &connections );

    private Q_SLOTS:
        void slotSelectionChanged();

    private:
        void setupUi();
        void setupActions();

        InfinoteManager *infinoteManager;

};

/**
 * @brief Widget to edit and control a connection.
 */
class ConnectionEditorWidget
    : public QWidget
{

    Q_OBJECT

    public:
        ConnectionEditorWidget( QWidget *parent = 0 );
        ConnectionEditorWidget( Connection &connection, QWidget *parent = 0 );

        /**
         * @brief Get connection being edited.
         * @return Connection being edited.  Returns if no connection is set.
         */
        Connection *getConnection() const;

    public Q_SLOTS:
        /**
         * @brief Set the connection being edited.
         * @param connection New connection to edit
         */
        void setConnection( Connection &connection );
        /**
         * @brief Clear connecting being editing, disables editing.
         */
        void unsetConnection();
        /**
         * @brief Save changes made to connection.
         */
        void saveChanges();

    private:
        void setEditable( bool );

        Connection *connection;
        Ui::ConnectionEditor *ui;
        bool is_editable;

};

/**
 * @brief A dialog to add a new connection.
 *
 * This dialog emits the signal addConnection when the user selects Ok.  It does not
 * actually handle the adding or creating of a connection in any way.
 */
class AddConnectionDialog
    : public KDialog
{
    
    Q_OBJECT
    
    public:
        AddConnectionDialog( QWidget *parent = 0 );
    
    Q_SIGNALS:
        /**
         * @brief The user is attempting to add a connection.
         */
        void addConnection( const QString &label, const QString &hostname, unsigned int port );
    
    private Q_SLOTS:
        void slotLocationChanged( const QString &text );
        void tryConnecting();
    
    private:
        void setupUi();
        void setupActions();
        
        Ui::AddConnectionDialog *ui;
    
}; // class AddConnectionDialog

/**
 * @brief Widget containing a ConnectionList and add / remove buttons.
 *
 * You can use several instances of this widget per InfinoteManager instance and
 * all will be synchronized.
 */
class ConnectionManagerWidget
    : public QWidget
{

    Q_OBJECT

    public:
        ConnectionManagerWidget( InfinoteManager &infinoteManager, QWidget *parent = 0 );

        /**
         * @brief Get the ConnectionListWidget this manager represents.
         */
        ConnectionListWidget &getListWidget() const;

    private Q_SLOTS:
        void slotAddConnection();
        void slotAddConnectionFinished();
        void slotRemoveConnection();
        void slotControlConnection();
        void slotItemSelectionChanged();

    private:
        void setupUi();
        void setupActions();

        InfinoteManager *infinoteManager;
        ConnectionListWidget *connectionListWidget;
        AddConnectionDialog *addConnectionDialog;
        QPushButton *addButton;
        QPushButton *removeButton;
        QPushButton *controlButton;

};

/**
 * @brief A dialog to manage the active connections.
 */
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
