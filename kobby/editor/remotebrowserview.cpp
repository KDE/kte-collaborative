#include "remotebrowserview.h"
#include "itemfactory.h"

#include <libqinfinity/browsermodel.h>

#include <KAction>
#include <KIcon>
#include <KLocalizedString>
#include <KToolBar>

#include <QTreeView>
#include <QVBoxLayout>
#include <QItemSelection>
#include <QModelIndexList>

#include "remotebrowserview.moc"

namespace Kobby
{

RemoteBrowserView::RemoteBrowserView( QInfinity::BrowserModel &model,
    QWidget *parent )
    : QWidget( parent )
    , m_treeView( new QTreeView( this ) )
    , browserModel( &model )
{
    m_treeView->setModel( &model );
    connect( m_treeView, SIGNAL(expanded(const QModelIndex&)),
        browserModel, SLOT(itemActivated(const QModelIndex&)) );
    connect( m_treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(slotSelectionChanged(const QItemSelection&, const QItemSelection&)) );
    setupActions();
    setupToolbar();

    QVBoxLayout *vertLayout = new QVBoxLayout( this );
    vertLayout->addWidget( toolBar );
    vertLayout->addWidget( m_treeView );

    setLayout( vertLayout );
}

void RemoteBrowserView::slotNewConnection()
{
    emit(createConnection());
}

void RemoteBrowserView::slotNewDocument()
{
}

void RemoteBrowserView::slotNewFolder()
{
}

void RemoteBrowserView::slotOpen()
{
}

void RemoteBrowserView::slotDelete()
{
    if( !m_treeView->selectionModel()->hasSelection() )
        return;
    QList<QModelIndex> indexes = m_treeView->selectionModel()->selectedIndexes();
    QList<QModelIndex>::Iterator indexItr;
    QStandardItem *item;
    for( indexItr = indexes.begin(); indexItr != indexes.end(); indexItr++ )
    {
        item = browserModel->itemFromIndex( *indexItr );
        browserModel->removeRow( indexItr->row() );
    }
}

void RemoteBrowserView::slotSelectionChanged( const QItemSelection &selected,
    const QItemSelection &deselected )
{
    Q_UNUSED(deselected)
    createDocumentAction->setEnabled( canCreateDocument( selected.indexes() ) );
    createFolderAction->setEnabled( canCreateDocument( selected.indexes() ) );
    openAction->setEnabled( canOpenItem( selected.indexes() ) );
    deleteAction->setEnabled( canDeleteItem( selected.indexes() ) );
}

void RemoteBrowserView::setupActions()
{
    createConnectionAction = new KAction( i18n("New Connection"), this );
    createDocumentAction = new KAction( i18n("New Document"), this );
    createFolderAction = new KAction( i18n("New Folder"), this );
    openAction = new KAction( i18n("Open Document"), this );
    deleteAction = new KAction( i18n("Delete"), this );

    createConnectionAction->setIcon( KIcon("network-connect.png") );
    createDocumentAction->setIcon( KIcon("document-new.png") );
    createFolderAction->setIcon( KIcon("folder-new.png") );
    openAction->setIcon( KIcon("document-open.png") );
    deleteAction->setIcon( KIcon("user-trash.png") );

    createConnectionAction->setEnabled( true );
    createDocumentAction->setEnabled( false );
    createFolderAction->setEnabled( false );
    openAction->setEnabled( false );
    deleteAction->setEnabled( false );

    connect( createConnectionAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewConnection()) );
    connect( createDocumentAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewDocument()) );
    connect( createFolderAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewFolder()) );
    connect( openAction, SIGNAL(triggered(bool)),
        this, SLOT(slotOpen()) );
    connect( deleteAction, SIGNAL(triggered(bool)),
        this, SLOT(slotDelete()) );
}

void RemoteBrowserView::setupToolbar()
{
    toolBar = new KToolBar( this );
    toolBar->setIconDimensions( 16 );
    toolBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
    toolBar->addAction( createConnectionAction );
    toolBar->addAction( createDocumentAction );
    toolBar->addAction( createFolderAction );
    toolBar->addAction( openAction );
    toolBar->addAction( deleteAction );
}

bool RemoteBrowserView::canCreateDocument( QModelIndexList selected )
{
    QStandardItem *item;
    QInfinity::NodeItem *nodeItem;
    if( selected.size() != 1 )
        return false;
    item = browserModel->itemFromIndex( selected[0] );
    if( !item )
        return false;
    if( item->type() == QInfinity::BrowserItemFactory::ConnectionItem )
        return true;
    if( item->type() == QInfinity::BrowserItemFactory::NodeItem )
    {
        nodeItem = dynamic_cast<QInfinity::NodeItem*>(item);
        return nodeItem->isDirectory();
    }
    return false;
}

bool RemoteBrowserView::canCreateFolder( QModelIndexList selected )
{
    return canCreateDocument( selected );
}

bool RemoteBrowserView::canOpenItem( QModelIndexList selected )
{
    QStandardItem *item;
    QModelIndexList::Iterator itr;
    if( selected.size() == 0 )
        return false;

    for( itr = selected.begin(); itr != selected.end(); itr++ )
    {
        item = browserModel->itemFromIndex( *itr );
        if( !item ||
            !(item->type() & QInfinity::BrowserItemFactory::NoteItem) )
            return false;
    }

    return true;
}

bool RemoteBrowserView::canDeleteItem( QModelIndexList selected )
{
    return selected.size() != 0;
}

}

