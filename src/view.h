#ifndef VIEW_H
#define VIEW_H

#include <kxmlguiclient.h>
#include <ktexteditor/view.h>

#include <QtCore/QObject>

namespace Kobby
{

class View
    : public QObject, public KXMLGUIClient
{

public:
    View(KTextEditor::View *view = 0);

}; // class View

} // namespace Kobby

#endif

