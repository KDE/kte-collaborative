#include "view.h"
#include "plugin.h"

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

