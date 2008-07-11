#include "view.h"

namespace Kobby
{

View::View(KTextEditor::View *view)
    : QObject(view)
    , KXMLGUIClient(view)
{
}

} // namespace Kobby

