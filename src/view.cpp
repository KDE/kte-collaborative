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

View::View(KTextEditor::View *view)
    : QObject(view)
    , KXMLGUIClient(view)
{
    setComponentData(PluginFactory::componentData());
    setupActions();
}

void View::setupActions()
{
    KAction *joinAction = new KAction(i18n("Join Kobby Session"), this);
    actionCollection()->addAction("kobby_join", joinAction);
    
    connect(joinAction, SIGNAL(triggered()), this, SLOT(slotJoinSession()));
    
    setXMLFile("kobbyui.rc");
}

void View::slotJoinSession()
{
    kDebug() << "Trying to join session.";
}

} // namespace Kobby

