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

#include "plugin.h"
#include "view.h"

#include <KDebug>

namespace Kobby
{

View::View( KTextEditor::View *view )
    : QObject(view)
    , KXMLGUIClient( view )
    , sessionManager( 0 )
{
    setComponentData( PluginFactory::componentData() );
    setupActions();
}

View::~View()
{
    if( sessionManager )
        sessionManager->close();
}

void View::setupActions()
{
    KAction *manageSessionsAction = new KAction( i18n( "Manage Kobby sessions" ), this );
    actionCollection()->addAction( "sessions_kobby_manage", manageSessionsAction );
    
    connect( manageSessionsAction, SIGNAL( triggered() ), this, SLOT( slotManageSessions() ) );
    
    setXMLFile( "kobbyui.rc" );
}

void View::slotManageSessions()
{
    if( sessionManager )
    {
        kDebug() << "Session manager already open.";
        return;
    }
    
    sessionManager = new SessionManager();
    sessionManager->setVisible( true );
    
    connect( sessionManager, SIGNAL( finished() ), this, SLOT( slotSessionManagerFinished() ) );
}

void View::slotSessionManagerFinished()
{
        sessionManager = 0;
}

} // namespace Kobby

#include "view.moc"

