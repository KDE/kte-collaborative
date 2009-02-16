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
    for( indexItr = indexes.begin(); indexItr != indexes.end(); indexItr++ )
    {
        browserModel->removeRow( indexItr->row() );
    }
}

void RemoteBrowserView::slotSelectionChanged( const QItemSelection &selected,
    const QItemSelection &deselected )
{
    QList<QModelIndex> indexes = selected.indexes();
    if( indexes.size() != 1 )
    {
        createDocumentAction->setEnabled( false );
        createFolderAction->setEnabled( false );
        createDocumentAction->setEnabled( false );
        openAction->setEnabled( false );
        deleteAction->setEnabled( indexes.size() != 0 );
        return;
    }

    QStandardItem *item = browserModel->itemFromIndex( indexes[0] );
    QInfinity::NodeItem *nodeItem;
    switch( item->type() )
    {
        case QInfinity::BrowserItemFactory::ConnectionItem:
            createDocumentAction->setEnabled( true );
            createFolderAction->setEnabled( true );
            openAction->setEnabled( false );
            deleteAction->setEnabled( true );
            break;
        case QInfinity::BrowserItemFactory::NodeItem:
            nodeItem = dynamic_cast<QInfinity::NodeItem*>(item);
            if( nodeItem->isDirectory() )
            {
                createDocumentAction->setEnabled( true );
                createFolderAction->setEnabled( true );
                openAction->setEnabled( false );
                deleteAction->setEnabled( true );
            }
            else
            {
                createDocumentAction->setEnabled( false );
                createFolderAction->setEnabled( false );
                openAction->setEnabled( true );
                deleteAction->setEnabled( true );
            }
    }
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

    createFolderAction->setEnabled( true );
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

}

