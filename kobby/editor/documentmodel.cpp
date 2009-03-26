#include "documentmodel.h"
#include "document.h"

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

