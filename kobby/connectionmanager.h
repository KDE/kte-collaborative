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

#ifndef KOBBY_SESSIONMANAGER_H
#define KOBBY_SESSIONMANAGER_H

#include "addconnectiondialog.h"
#include "infinotemanager.h"
#include "connectionlistwidgetitem.h"
#include "filebrowser.h"

#include <KDialog>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "ui_connectionmanager.h"

namespace Infinity
{
    class XmppConnection;
}

namespace Kobby
{

class ConnectionManager
    : public KDialog
{
    
    Q_OBJECT
    
    public:
        ConnectionManager( InfinoteManager &manager, QWidget *parent = 0 );
        ~ConnectionManager();
    
    public Q_SLOTS:
        //void addConnection( Infinity::XmppConnection &conn, const QString &hostname );
        void addConnection( const QString name, const QString hostname, unsigned int port );
    
    private Q_SLOTS:
        void slotAddConnectionDialog();
        void slotAddConnectionDialogFinished();
        void slotFileBrowser();
        void slotSelectionChanged();
        void slotRemoveSelectedItems();
    
    private:
        void setupActions();
        
        AddConnectionDialog *addConnectionDialog;
        FileBrowserDialog *fileBrowserDialog;
        InfinoteManager *infinoteManager;
        Ui::ConnectionManager ui;
    
}; // class ConnectionManager

} // namespace Kobby

#endif
