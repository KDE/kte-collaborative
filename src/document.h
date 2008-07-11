#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QtCore/QObject>

#include <ktexteditor/document.h>
#include <kxmlguiclient.h>

namespace Kobby
{

class Document
    : public QObject, public KXMLGUIClient
{

public:
    explicit Document(KTextEditor::Document *document = 0);

};

} // namespace Kobby

#endif

