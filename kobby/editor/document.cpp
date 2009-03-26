#include "document.h"

namespace Kobby
{

Document::Document( KTextEditor::Document &kDocument,
    QObject *parent )
    : QObject( parent )
    , m_kDocument( &kDocument )
{
    m_kDocument->setParent( this );
}

Document::~Document()
{
}

Document::Type Document::type() const
{
    return KDocument;
}

KTextEditor::Document *Document::kDocument() const
{
    return m_kDocument;
}

bool Document::save()
{
    return m_kDocument->documentSave();
}

QString Document::name()
{
    return m_kDocument->documentName();
}

InfTextDocument::InfTextDocument( KTextEditor::Document &kDocument,
    QInfinity::Session &session,
    QObject *parent )
    : Document( kDocument, parent )
    , m_session( &session )
{
}

Document::Type InfTextDocument::type() const
{
    return Document::InfText;
}

void InfTextDocument::setupSignals()
{
}

}

