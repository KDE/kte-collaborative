#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientbrowseriter.h>

#include "filebrowserwidget.h"
#include "browsermodel.h"
#include "browseritem.h"
#include "createitemdialog.h"

#include <libqinfinitymm/browser.h>
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

FileBrowserWidget::FileBrowserWidget( BrowserModel &model,
    QWidget *parent )
    : QWidget( parent )
    , fileModel( &model )
{
    setupUi();
    setupActions();
}

void FileBrowserWidget::createFolder( QInfinity::BrowserFolderItem &parent,
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

void FileBrowserWidget::createNote( QInfinity::BrowserFolderItem &parent,
    Infinity::ClientNotePlugin &notePlugin,
    QString name )
{
    Infinity::ClientBrowserIter iter = parent.iter();
    Infinity::ClientBrowser *browser = iter.getBrowser();

    browser->addNote( iter,
        name.toAscii(),
        &notePlugin,
        false );
}

void FileBrowserWidget::slotItemActivated( const QModelIndex &index )
{
    QStandardItem *standardItem = fileModel->itemFromIndex( index );

    if( !standardItem )
    {
        kDebug() << "Got invalid item when locating activated index!";
        return;
    }

    if( standardItem->type() == QInfinity::BrowserItem::Note )
    {
        BrowserNoteItem *noteItem = dynamic_cast<BrowserNoteItem*>(standardItem);
        QInfinity::BrowserConnectionItem *parentConnection = fileModel->nodeParentConnection( noteItem->iter() );
        parentConnection->connection().browser()->clientBrowser().subscribeSession( noteItem->iter() );
    }
}

void FileBrowserWidget::contextMenuEvent( QContextMenuEvent *e )
{
    QMenu *menu = new QMenu( this );

    menu->addAction( createFolderAction );
    menu->addAction( createNoteAction );
    menu->addAction( joinNoteAction );
    menu->addAction( deleteItemAction );

    menu->popup( e->globalPos() );
}

void FileBrowserWidget::slotSelectionChanged( const QItemSelection &selected,
    const QItemSelection &deselected )
{
    bool is_single_selected = false;
    bool can_have_children = false;
    QModelIndex index;
    QStandardItem *item = 0;

    if( selected.indexes().size() == 0 )
    {
        createFolderAction->setEnabled( false );
        deleteItemAction->setEnabled( false );
        createNoteAction->setEnabled( false );
        joinNoteAction->setEnabled( false );
    }
    else
    {
        is_single_selected = selected.indexes().size() == 1;
        index = selected.indexes().at(0);
        item = fileModel->itemFromIndex( index );
        can_have_children = canHaveChildren( index );

        deleteItemAction->setEnabled( true );
        createFolderAction->setEnabled( is_single_selected
            && can_have_children );
        createNoteAction->setEnabled( is_single_selected
      && can_have_children );
        joinNoteAction->setEnabled( is_single_selected
            && item->type() == QInfinity::BrowserItem::Note );
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
    QStandardItem *item = getSingleSelectedItem();
    QInfinity::BrowserFolderItem *folderItem = 0;

    if( !item )
        return;

    switch( item->type() )
    {
        case QInfinity::BrowserItem::Connection:
            folderItem = dynamic_cast<QInfinity::BrowserConnectionItem*>(item)->rootFolder();
            break;
        case QInfinity::BrowserItem::Folder:
            folderItem = dynamic_cast<QInfinity::BrowserFolderItem*>(item);
    }

    if( !folderItem )
        return;

    CreateItemDialog *dialog = new CreateItemDialog( *folderItem, this );
    connect( dialog, SIGNAL(create( QInfinity::BrowserFolderItem&, QString )),
        this, SLOT(createFolder( QInfinity::BrowserFolderItem&, QString )) );
    dialog->exec();
}

void FileBrowserWidget::slotCreateNote()
{
    QStandardItem *item = getSingleSelectedItem();
    QInfinity::BrowserFolderItem *folderItem = 0;

    if( !item )
        return;

    switch( item->type() )
    {
        case QInfinity::BrowserItem::Connection:
            folderItem = dynamic_cast<QInfinity::BrowserConnectionItem*>(item)->rootFolder();
            break;
        case QInfinity::BrowserItem::Folder:
            folderItem = dynamic_cast<QInfinity::BrowserFolderItem*>(item);
    }

    if( !folderItem )
        return;

    CreateItemDialog *dialog = new CreateItemDialog( *folderItem, this );
    connect( dialog, SIGNAL(create( QInfinity::BrowserFolderItem&, QString )),
        this, SLOT(createNote( QInfinity::BrowserFolderItem&, QString )) );
    dialog->exec();
}

void FileBrowserWidget::setupUi()
{
    createFolderAction = new KAction( KIcon( "folder-new.png" ) , tr("Create Folder"), this );
    createFolderAction->setEnabled( false );
    createNoteAction = new KAction( KIcon( "document-new.png" ), tr("Create Note"), this );
    createNoteAction->setEnabled( false );
    joinNoteAction = new KAction( KIcon( "document-open.png" ), tr("Join Note"), this );
    joinNoteAction->setEnabled( false );
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
    connect( createNoteAction, SIGNAL(triggered()),
        this, SLOT(slotCreateNote()) );

    connect( m_treeView, SIGNAL(expanded( const QModelIndex& )),
        fileModel, SLOT(activateItem( const QModelIndex& )) );
    connect( m_treeView, SIGNAL(doubleClicked( const QModelIndex& )),
        this, SLOT(slotItemActivated( const QModelIndex& )) );
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

QStandardItem *FileBrowserWidget::getSingleSelectedItem()
{
    QItemSelectionModel *selectionModel = m_treeView->selectionModel();
    QStandardItem *item;
    if( !selectionModel )
        return 0;
    QList<QModelIndex> selectedIndexes = selectionModel->selectedRows();

    if( selectedIndexes.size() == 0 || selectedIndexes.size() > 1 )
        return 0;

    item = fileModel->itemFromIndex( selectedIndexes.at(0) );

    return item;
}

}

