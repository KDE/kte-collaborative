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
    connect( this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
        this, SLOT(slotRowsAboutRemoved(QModelIndex,int,int)) );
}

void DocumentModel::insertDocument( Document &document )
{
    DocumentItem *item = new DocumentItem( document );
    appendRow( item );
    emit( documentAdded( document ) );
}

void DocumentModel::slotRowsAboutRemoved( const QModelIndex &parent, int start, int end )
{
    QStandardItem *item = itemFromIndex( index( start, 0, parent ) );
    if( item->type() != DocumentItem::KobbyDocument )
    {
        kDebug() << "Removing non-document item from document model!";
        return;
    }
    DocumentItem *docItem = dynamic_cast<DocumentItem*>(item);
    emit( documentAboutToBeRemoved(docItem->document()) );
}

}

