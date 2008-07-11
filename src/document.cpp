#include "document.h"

namespace Kobby
{

Document::Document(KTextEditor::Document *document)
    : QObject(document), KXMLGUIClient(document)
{
}

} // namespace Kobby

