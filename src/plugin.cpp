#include "plugin.h"
#include "document.h"
#include "view.h"

#include <KDebug>

#include <QtCore/QListIterator>

#include <sigc++/signal.h>

namespace Kobby
{

K_PLUGIN_FACTORY_DEFINITION(PluginFactory,
    registerPlugin<Plugin>("ktexteditor_kobby");
    )

K_EXPORT_PLUGIN(PluginFactory("ktexteditor_kobby", "ktexteditor_plugins"))

Plugin::Plugin(QObject *parent, const QVariantList &args)
    : KTextEditor::Plugin(parent)
{
    Q_UNUSED(args)

}

void Plugin::addDocument(KTextEditor::Document *document)
{
    Document *ndocument = new Document(document);
    m_documents.append(ndocument);
}

void Plugin::removeDocument(KTextEditor::Document *document)
{
    int i;

    // Not using iterator because we need the index to remove
    for (i = 0; i < m_documents.size(); i++)
    {
        if (m_documents.at(i)->parentClient() == document)
        {
            Document *ndocument = m_documents.at(i);
            m_documents.removeAt(i);
            delete ndocument;
        }
    }
}

void Plugin::addView(KTextEditor::View *view)
{
    View *nview = new View(view);
    m_views.append(nview);
}

void Plugin::removeView(KTextEditor::View *view)
{
    int i;

    for (i = 0; i < m_views.size(); i++)
    {
        if (m_views.at(i)->parentClient() == view)
        {
            View *nview = m_views.at(i);
            m_views.removeAt(i);
            delete nview;
        }
    }
}

} // namespace Kobby

