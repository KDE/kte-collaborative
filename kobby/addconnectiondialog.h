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

#ifndef KOBBY_ADDCONNECTIONDIALOG_H
#define KOBBY_ADDCONNECTIONDIALOG_H

#include <KDialog>

#include <QWidget>
#include <QString>

#include "ui_addconnectiondialog.h"

namespace Infinity
{
    class XmppConnection;
}

namespace Kobby
{

class AddConnectionDialog
    : public KDialog
{
    
    Q_OBJECT
    
    public:
        AddConnectionDialog( QWidget *parent = 0 );
    
    Q_SIGNALS:
        void addConnection( const QString &label, const QString &hostname, unsigned int port );
    
    private Q_SLOTS:
        void slotLocationChanged( const QString &text );
        void tryConnecting();
    
    private:
        void setupActions();
        
        Ui::AddConnectionDialog ui;
    
}; // class AddConnectionDialog

} // namespace Kobby

#endif
