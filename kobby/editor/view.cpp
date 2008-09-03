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
#include "kobby/infinote/infinotemanager.h"
#include "kobby/dialogs/connectionmanager.h"

#include <KDebug>

namespace Kobby
{

View::View( InfinoteManager &manager, KTextEditor::View *view )
    : QObject(view)
    , KXMLGUIClient( view )
    , connectionManager( 0 )
    , infinoteManager( &manager )
{
    setComponentData( PluginFactory::componentData() );
    setupActions();
}

View::~View()
{
    if( connectionManager )
        connectionManager->close();
}

void View::setupActions()
{
    KAction *manageConnectionsAction = new KAction( KIcon( "network-workgroup.png" ), i18n( "Manage Connections" ), this );
    
    actionCollection()->addAction( "tools_kobby_manageconnections", manageConnectionsAction );
    connect( manageConnectionsAction, SIGNAL( triggered() ), this, SLOT( slotManageConnections() ) );
    
    setXMLFile( "kobbyui.rc" );
}

void View::slotManageConnections()
{
    if( connectionManager )
    {
        kDebug() << "Connection manager already open.";
        return;
    }
    
    connectionManager = new ConnectionManagerDialog( *infinoteManager );
    connectionManager->setVisible( true );
    
    connect( connectionManager, SIGNAL( finished() ), this, SLOT( slotConnectionManagerFinished() ) );
}

void View::slotConnectionManagerFinished()
{
        connectionManager = 0;
}

} // namespace Kobby

#include "view.moc"

