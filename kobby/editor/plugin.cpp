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

#include <libinfinitymm/init.h>

#include <kobby/editor/plugin.h>
#include <kobby/editor/document.h>
#include <kobby/editor/view.h>
#include <kobby/dialogs/settingsdialog.h>
#include <kobby/infinote/infinotemanager.h>

#include <KDebug>

#include <QtCore/QListIterator>

namespace Kobby
{

K_PLUGIN_FACTORY_DEFINITION( PluginFactory,
    registerPlugin<Plugin>( "ktexteditor_kobby" );
    registerPlugin<SettingsDialog>( "ktexteditor_kobby_settings" );
)

K_EXPORT_PLUGIN( PluginFactory( "ktexteditor_kobby", "ktexteditor_plugins" ) )

Plugin::Plugin( QObject *parent, const QVariantList &args )
    : KTextEditor::Plugin( parent )
{
    Q_UNUSED( args )

    infinoteManager = new InfinoteManager();
}

void Plugin::addDocument( KTextEditor::Document *document )
{
    Document *ndocument = new Document( document );
    m_documents.append( ndocument );
}

void Plugin::removeDocument( KTextEditor::Document *document )
{
    int i;

    // Not using iterator because we need the index to remove
    for( i = 0; i < m_documents.size(); i++ )
    {
        if (m_documents.at( i )->parentClient() == document)
        {
            Document *ndocument = m_documents.at( i );
            m_documents.removeAt( i );
            delete ndocument;
        }
    }
}

void Plugin::addView( KTextEditor::View *view )
{
    View *nview = new View( *infinoteManager, view);
    m_views.append(nview);
}

void Plugin::removeView( KTextEditor::View *view )
{
    int i;

    for( i = 0; i < m_views.size(); i++ )
    {
        if( m_views.at( i )->parentClient() == view )
        {
            View *nview = m_views.at( i );
            m_views.removeAt( i );
            delete nview;
        }
    }
}

} // namespace Kobby

