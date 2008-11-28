#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientbrowseriter.h>
#include <libqinfinitymm/browsermodel.h>
#include "filebrowserwidget.h"
#include "createfolderdialog.h"

#include <libqinfinitymm/browseritem.h>
#include <libqinfinitymm/infinotemanager.h>

#include <KIcon>
#include <KAction>
#include <KDebug>

#include <QList>
#include <QString>
#include <QVBoxLayout>
#include <QTreeView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QItemSelectionModel>
#include <QItemSelection>

#include "filebrowserwidget.moc"

namespace Kobby
{

FileBrowserWidget::FileBrowserWidget( QWidget *parent )
    : QWidget( parent )
    , fileModel( new QInfinity::BrowserModel( this ) )
{
    setupUi();
    setupActions();
}

void FileBrowserWidget::createFolder( const QInfinity::BrowserFolderItem &parent,
    QString name )
{
    Infinity::ClientBrowserIter iter = parent.iter();
    Infinity::ClientBrowser *browser = iter.getBrowser();
    if( !browser )
    {
        kDebug() << "parent iterator for adding folder does not reference a browser!";
        return;
    }

    browser->addSubdirectory( iter, name.toAscii() );
}

void FileBrowserWidget::contextMenuEvent( QContextMenuEvent *e )
{
    QMenu *menu = new QMenu( this );

    menu->addAction( createFolderAction );
    menu->addAction( deleteItemAction );

    menu->popup( e->globalPos() );
}

void FileBrowserWidget::slotSelectionChanged( const QItemSelection &selected,
    const QItemSelection &deselected )
{
    if( selected.indexes().size() == 0 )
    {
        createFolderAction->setEnabled( false );
        deleteItemAction->setEnabled( false );
    }
    else
    {
        deleteItemAction->setEnabled( true );
        createFolderAction->setEnabled( selected.indexes().size() == 1
            && canHaveChildren( selected.indexes().at(0) ) );
    }
}

void FileBrowserWidget::slotDeleteSelected()
{
    QItemSelectionModel *selectionModel = m_treeView->selectionModel();
    if( !selectionModel )
        return;
    QList<QModelIndex> selectedIndexes = selectionModel->selectedRows();
    QList<QModelIndex>::Iterator itr;
    QStandardItem *item;

    for( itr = selectedIndexes.begin(); itr != selectedIndexes.end(); ++itr )
    {
        item = fileModel->itemFromIndex( *itr );
        if( !item )
        {
            kDebug() << "Got bad item back to delete, skipping.";
            continue;
        }

        switch( item->type() )
        {
            case QInfinity::BrowserItem::Connection:
                QInfinity::InfinoteManager::instance()->removeConnection(
                    dynamic_cast<QInfinity::BrowserConnectionItem*>(item)->connection() );
                break;
            case QInfinity::BrowserItem::Note:
            case QInfinity::BrowserItem::Folder:
                QInfinity::BrowserNodeItem *nodeItem = dynamic_cast<QInfinity::BrowserNodeItem*>(item);
                nodeItem->iter().getBrowser()->removeNode( nodeItem->iter() );
        }
    }
}

void FileBrowserWidget::slotCreateFolder()
{
    QItemSelectionModel *selectionModel = m_treeView->selectionModel();
    QStandardItem *item;
    QInfinity::BrowserFolderItem *folderItem;
    if( !selectionModel )
        return;
    QList<QModelIndex> selectedIndexes = selectionModel->selectedRows();

    if( selectedIndexes.size() == 0 || selectedIndexes.size() > 1 )
        return;

    item = fileModel->itemFromIndex( selectedIndexes.at(0) );

    switch( item->type() )
    {
        case QInfinity::BrowserItem::Connection:
            folderItem = dynamic_cast<QInfinity::BrowserConnectionItem*>(item)->rootFolder();
            break;
        case QInfinity::BrowserItem::Folder:
            folderItem = dynamic_cast<QInfinity::BrowserFolderItem*>(item);
    }

    CreateFolderDialog *dialog = new CreateFolderDialog( *folderItem, this );
    connect( dialog, SIGNAL(createFolder( const QInfinity::BrowserFolderItem&, QString )),
        this, SLOT(createFolder( const QInfinity::BrowserFolderItem&, QString )) );
    dialog->exec();
}

void FileBrowserWidget::setupUi()
{
    createFolderAction = new KAction( KIcon( "folder-new.png" ) , tr("Create Folder"), this );
    createFolderAction->setEnabled( false );
    deleteItemAction = new KAction( KIcon( "edit-delete.png" ), tr("delete"), this );
    deleteItemAction->setEnabled( false );

    QVBoxLayout *vertLayout = new QVBoxLayout( this );
    m_treeView = new QTreeView( this );
    m_treeView->setModel( fileModel );
    m_treeView->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_treeView->setSelectionMode( QAbstractItemView::ExtendedSelection );

    vertLayout->addWidget( m_treeView );
    setLayout( vertLayout );

}

void FileBrowserWidget::setupActions()
{
    QItemSelectionModel *selectionModel = m_treeView->selectionModel();
    if( selectionModel )
    {
        connect( selectionModel, SIGNAL(selectionChanged( const QItemSelection, const QItemSelection )),
            this, SLOT(slotSelectionChanged( const QItemSelection&, const QItemSelection& )) );
    }
    connect( deleteItemAction, SIGNAL(triggered()),
        this, SLOT(slotDeleteSelected()) );
    connect( createFolderAction, SIGNAL(triggered()),
        this, SLOT(slotCreateFolder()) );

    connect( m_treeView, SIGNAL(doubleClicked( const QModelIndex& )),
        fileModel, SLOT(openItem( const QModelIndex& )) );
    connect( m_treeView, SIGNAL(expanded( const QModelIndex& )),
        fileModel, SLOT(openItem( const QModelIndex& )) );
}

bool FileBrowserWidget::canHaveChildren( const QModelIndex &index )
{
    QStandardItem *item = fileModel->itemFromIndex(index);
    if( !item )
        return false;
    switch( item->type() )
    {
        case QInfinity::BrowserItem::Connection:
        case QInfinity::BrowserItem::Folder:
            return true;
        default:
            return false;
    }
}

}

