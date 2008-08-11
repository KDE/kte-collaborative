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

#ifndef KOBBY_CONFIGDIALOG_H
#define KOBBY_CONFIGDIALOG_H

#include "plugin.h"

#include <KDialog>
#include <KCModule>

#include <QListWidget>
#include <QListWidgetItem>
#include <QSplitter>

namespace Kobby
{

class ConfigDialogList
    : public QListWidget
{
    
    enum Items
    {
        User,
        Sessions
    };
    
    Q_OBJECT
    
    public:
        ConfigDialogList();
    
    public Q_SLOTS:
        void slotItemSelectionChanged();
    
    Q_SIGNALS:
        void itemSelected( int item );
        
    private:
        QListWidgetItem userItem;
        QListWidgetItem sessionsItem;
};

class ConfigDialog
    : public KCModule
{
    
    Q_OBJECT
    
    public:
        ConfigDialog( QWidget *parent = 0, const QVariantList &args = QVariantList() );
    
    private:
        QSplitter mainSplitter;
        QTabWidget tabWidget;
    
};

} // namespace Kobby

#endif
