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

#ifndef KOBBY_VIEW_H
#define KOBBY_VIEW_H

#include <kobby/dialogs/settingsdialog.h>

#include <KXMLGUIClient>
#include <KTextEditor/View>
#include <KLocale>
#include <KAction>
#include <KActionCollection>

#include <QtCore/QObject>

namespace Kobby
{

class ConnectionManagerDialog;
class InfinoteManager;

class View
    : public QObject
    , public KXMLGUIClient
{
    Q_OBJECT
    
    public:
        View( InfinoteManager &manager, KTextEditor::View *view = 0 );
        ~View();
    
        void setupActions();
    
    private Q_SLOTS:
        void slotManageConnections();
        void slotConnectionManagerFinished();
    
    private:
        KTextEditor::View *view;
        ConnectionManagerDialog *connectionManager;
        InfinoteManager *infinoteManager;
    
}; // class View

} // namespace Kobby

#endif

