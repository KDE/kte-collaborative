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

#include <libqinfinity/browseriter.h>
#include <libqinfinity/browseritemfactory.h>

#include <KMessageBox>
#include <KLocalizedString>
#include <KDebug>

#include <QPointer>

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

        DocumentItem( Document &doc,
            unsigned int node_id = 0 );
        ~DocumentItem();

        int type() const;
        Document &document() const;
        void setCollaborative(bool val);
        bool collaborative() const;

        /* Only valid if collaborative() is true */
        unsigned int nodeId() const;

    private:
        Document *m_document;
        bool m_collaborative;
        int m_nodeId;

};

DocumentItem::DocumentItem( Document &doc,
    unsigned int node_id )
    : m_document( &doc )
    , m_collaborative( false )
    , m_nodeId( node_id )
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

void DocumentItem::setCollaborative(bool val)
{
    m_collaborative = val;
}

bool DocumentItem::collaborative() const
{
    return m_collaborative;
}

unsigned int DocumentItem::nodeId() const
{
    return m_nodeId;
}

DocumentModel *DocumentModel::instance()
{
    static DocumentModel *inst;
    if(!inst)
        inst = new DocumentModel();
    return inst;
}

DocumentModel::DocumentModel( QObject *parent )
    : QStandardItemModel( parent )
{
    connect(this, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&,
            int, int)),
        this, SLOT(slotRowsAboutToBeRemoved(const QModelIndex&,
            int, int)));
}

void DocumentModel::removeDocument( Document &doc,
    bool dont_warn )
{
    removeDocument( *doc.kDocument(), dont_warn );
}

void DocumentModel::removeDocument( KTextEditor::Document& kDoc,
    bool dont_warn )
{
    DocumentItem *di = m_kDocumentItemWrappers[&kDoc];
    if( di )
    {
        if( dont_warn || KMessageBox::warningYesNo( 0, i18n( "Are you sure you want to "\
            "close this document?" ) ) == KMessageBox::Yes )
            removeRow( di->row(), QModelIndex() );
    }
}

void DocumentModel::removeDocuments( QList<QModelIndex> indexes,
    bool dont_warn )
{
    QString warnMsg;

    if( indexes.length() > 1 )
    {
        warnMsg.append( i18n( "You are about to close %1 documents.", indexes.length() ) );
        warnMsg.append( " " );
        warnMsg.append( i18n( "Are you sure you want to do this?" ) );
    }
    else
        warnMsg.append( i18n( "Are you sure you want to close this document?" ) );

    if( dont_warn || KMessageBox::warningYesNo( 0, warnMsg ) == KMessageBox::Yes )
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

Document *DocumentModel::documentFromNodeItem( QInfinity::NodeItem &item )
{
    DocumentItem *di = m_infNodeToDocumentItem[item.iter().id()];
    if( di )
        return &di->document();
    else
        return 0;
}

QList<Document*> DocumentModel::dirtyDocs()
{
    QList<Document*> li;
    Document *doc;
    DocumentItem *docItem;
    QStandardItem *stdItem;
    QStandardItem *rootItem = invisibleRootItem();
    int i;
    for(i = 0;(stdItem = rootItem->child(i, 0));i++)
    {
        docItem = dynamic_cast<DocumentItem*>(stdItem);
        if( !docItem )
        {
            kDebug() << i18n( "Non DocumentItem in DocumentModel." );
            continue;
        }
        doc = &docItem->document();
        if( doc->type() == Document::KDocument
            && doc->isDirty() )
            li.append( doc );
    }
    return li;
}

QList<Document*> DocumentModel::collabDocs()
{
    QList<Document*> li;
    Document *doc;
    DocumentItem *docItem;
    QStandardItem *stdItem;
    QStandardItem *rootItem = invisibleRootItem();
    int i;
    for(i = 0;(stdItem = rootItem->child(i, 0));i++)
    {
        docItem = dynamic_cast<DocumentItem*>(stdItem);
        if( !docItem )
        {
            kDebug() << i18n( "Non DocumentItem in DocumentModel." );
            continue;
        }
        doc = &docItem->document();
        if( doc->type() == Document::InfText )
            li.append( doc );
    }
    return li;
}

void DocumentModel::insertDocument( Document &document,
    const QInfinity::BrowserIter *iter )
{
    // Item takes ownership of document
    DocumentItem *item;
    if( !iter )
        item = new DocumentItem( document );
    else
    {
        item = new DocumentItem( document, iter->id() );
        item->setCollaborative(true);
        m_infNodeToDocumentItem[item->nodeId()] = item;
    }
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
    Q_UNUSED(parent);
    DocumentItem *rm;
    while( start <= end )
    {
        rm = dynamic_cast<DocumentItem*>(item(start));
        if( rm )
        {
            emit(documentAboutToBeRemoved(rm->document()));
            m_kDocumentItemWrappers.remove(rm->document().kDocument());
            if( rm->collaborative() )
            {
                m_infNodeToDocumentItem.remove(rm->nodeId());
            }
        }
        else
        {
            kDebug() << i18n("Row removed from document model not of DocumentItem type!");
        }
        start++;
    }
}

void DocumentModel::slotDocumentFatalError( Kobby::Document* document, QString message )
{
    Q_UNUSED(message);
    removeDocument( *document->kDocument(), true );
}

}

