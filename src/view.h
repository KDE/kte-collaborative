#ifndef VIEW_H
#define VIEW_H

#include <KXMLGUIClient>
#include <KTextEditor/View>
#include <KLocale>
#include <KAction>
#include <KActionCollection>

#include <QtCore/QObject>

namespace Kobby
{

class View
    : public QObject, public KXMLGUIClient
{
    Q_OBJECT

public:
    View(KTextEditor::View *view = 0);
    
    void setupActions();

private Q_SLOTS:
    void slotJoinSession();

}; // class View

} // namespace Kobby

#endif

