#ifndef PLUGIN_H
#define PLUGIN_H

#include <ktexteditor/plugin.h>

#include <QtCore/QObject>
#include <QtCore/QList>

namespace Kobby
{

class Plugin
    : public KTextEditor::Plugin
{
public:
    explicit Plugin(QObject *parent = 0,
        const QVariantList &args = QVariantList());

    void addDocument(KTextEditor::Document *document);
    void removeDocument(KTextEditor::Document *document);
    void addView(KTextEditor::View *view);
    void removeView(KTextEditor::View *view);

private:
    QList<class View*> m_views;
    QList<class Document*> m_documents;

}; // class Plugin

} // namespace Kobby

#endif

