/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "documentmodel.h"
#include "document.h"

#include <KMessageBox>
#include <KLocalizedString>
#include <KDebug>

#include "documentmodel.moc"

namespace Kobby
{

class DocumentItem
    : public QObject
    , public QStandardItem
{
    public:
        enum Type
        {
            KobbyDocument = 1001
        };

        DocumentItem( Document &doc );
        ~DocumentItem();

        int type() const;
        Document &document() const;

    private:
        Document *m_document;

};

DocumentItem::DocumentItem( Document &doc )
    : m_document( &doc )
{
    setText( doc.name() );
}

DocumentItem::~DocumentItem()
{
    // This may be destroyed in Document::fatalError
    m_document->deleteLater();
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

void DocumentModel::removeDocument( KTextEditor::Document& kDoc,
    bool dont_warn )
{
    DocumentItem *di = m_kDocumentItemWrappers[&kDoc];
    if( di )
    {
        if( !dont_warn && KMessageBox::warningYesNo( 0, "Are you sure you want to "\
            "close this document?") == KMessageBox::Yes )
            removeRow( di->row(), QModelIndex() );
    }
}

void DocumentModel::removeDocuments( QList<QModelIndex> indexes,
    bool dont_warn )
{
    QString confirm = i18n( "You are about to close " ) +
        QString::number(indexes.length()) + i18n( " documents.  " )
        + i18n( "Are you sure you want to do this?" );
    if( !dont_warn && KMessageBox::warningYesNo( 0, confirm ) == KMessageBox::Yes )
    {
        QModelIndex index, parent;
        foreach( index, indexes )
        {
            parent = index.parent();
            removeRow( index.row(), parent );
        }
    }
}

Document *DocumentModel::documentFromIndex( const QModelIndex& index )
{
    DocumentItem *di = dynamic_cast<DocumentItem*>(itemFromIndex( index ));
    if( di )
        return &di->document();
    else
        return 0;
}

Document *DocumentModel::documentFromKDoc( KTextEditor::Document &kDoc )
{
    DocumentItem *di = m_kDocumentItemWrappers[&kDoc];
    if( di )
        return &di->document();
    else
        return 0;
}

void DocumentModel::insertDocument( Document &document )
{
    DocumentItem *item = new DocumentItem( document );
    m_kDocumentItemWrappers[document.kDocument()] = item;
    connect( &document, SIGNAL(fatalError( Document*, QString )),
        this, SLOT(slotDocumentFatalError( Document*, QString )) );
    connect( &document, SIGNAL(fatalError( Document*, QString )),
        this, SIGNAL(documentFatalError( Document*, QString )) );
    appendRow( item );
    emit( documentAdded( document ) );
}

void DocumentModel::slotRowsAboutToBeRemoved( const QModelIndex &parent,
    int start,
    int end )
{
    DocumentItem *rm;
    while( start <= end )
    {
        rm = dynamic_cast<DocumentItem*>(item(start));
        if( rm )
        {
            emit(documentAboutToBeRemoved(rm->document()));
            m_kDocumentItemWrappers.remove(rm->document().kDocument());
        }
        else
        {
            kDebug() << "Row removed from document model not of DocumentItem type!";
        }
        start++;
    }
}

void DocumentModel::slotDocumentFatalError( Kobby::Document* document, QString message )
{
    removeDocument( *document->kDocument() );
}


}

