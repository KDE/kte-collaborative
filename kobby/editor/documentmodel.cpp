#include "documentmodel.h"
#include "document.h"

#include <KDebug>

#include "documentmodel.moc"

namespace Kobby
{

DocumentItem::DocumentItem( Document &doc )
    : m_document( &doc )
{
    setText( doc.name() );
}

DocumentItem::~DocumentItem()
{
    delete m_document;
}

int DocumentItem::type() const
{
    return DocumentItem::KobbyDocument;
}

Document &DocumentItem::document() const
{
    return *m_document;
}

DocumentModel::DocumentModel( QObject *parent )
    : QStandardItemModel( parent )
{
}

void DocumentModel::insertDocument( Document &document )
{
    DocumentItem *item = new DocumentItem( document );
    appendRow( item );
    emit( documentAdded( document ) );
}

}

