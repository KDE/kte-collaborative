#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>
#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientbrowseriter.h>
#include <libinfinitymm/client/clientexplorerequest.h>

#include <qinfinitymm/qtio.h>

#include "filebrowser.h"

#include <kobby/infinote/connection.h>
#include <kobby/infinote/infinotemanager.h>
#include <kobby/dialogs/requestprogress.h>

#include <KDebug>
#include <KIcon>
#include <KPushButton>

#include <glib/gerror.h>

#include "kobby/ui_newfolderwidget.h"
#include "kobby/ui_filebrowserwidget.h"

namespace Kobby
{

FileBrowserWidgetItem::FileBrowserWidgetItem( QString name, const Infinity::ClientBrowserIter &iter, int type, QTreeWidget *parent )
    : QTreeWidgetItem( parent, type )
    , node( new Infinity::ClientBrowserIter() )
{
    *node = iter;
    setupUi();

    setText( 0, name );
}

FileBrowserWidgetItem::FileBrowserWidgetItem( const Infinity::ClientBrowserIter &iter, int type, QTreeWidget *parent )
    : QTreeWidgetItem( parent, type )
    , node( new Infinity::ClientBrowserIter() )
{
    *node = iter;
    setupUi();

    setText( 0, node->getName() );
}

Infinity::ClientBrowserIter &FileBrowserWidgetItem::getNode() const
{
    return *node;
}

void FileBrowserWidgetItem::setupUi()
{
    if( type() == Folder )
        setIcon( 0, KIcon( "folder.png" ) );
    else if( type() == Note )
        setIcon( 0, KIcon( "text-plain.png" ) );
}

FileBrowserWidgetNoteItem::FileBrowserWidgetNoteItem( Infinity::ClientBrowserIter &iter, QTreeWidget *parent )
    : FileBrowserWidgetItem( iter, FileBrowserWidgetItem::Note, parent )
{
}

FileBrowserWidgetFolderItem::FileBrowserWidgetFolderItem( Infinity::ClientBrowserIter &iter, QTreeWidget *parent )
    : FileBrowserWidgetItem( iter, FileBrowserWidgetItem::Folder, parent )
    , exploreRequest( new Glib::RefPtr<Infinity::ClientExploreRequest> )
    , is_populated( false )
{
    setupUi();
}

FileBrowserWidgetFolderItem::FileBrowserWidgetFolderItem( QString name, Infinity::ClientBrowserIter &iter, QTreeWidget *parent )
    : FileBrowserWidgetItem( name, iter, FileBrowserWidgetItem::Folder, parent )
    , exploreRequest( 0 )
    , is_populated( false )
{
    setupUi();
}

FileBrowserWidgetFolderItem::~FileBrowserWidgetFolderItem()
{
    /* reset causes segfault
    if( exploreRequest )
    {
        exploreRequest->reset();
        delete exploreRequest;
    }
    */
}

void FileBrowserWidgetFolderItem::populate( bool expand_when_finished )
{
    Q_UNUSED( expand_when_finished )

    if( is_populated )
        return;
    else if( getNode().isExplored() )
        return;
    else
    {
        exploreRequest = new Glib::RefPtr<Infinity::ClientExploreRequest>;
        *exploreRequest = getNode().explore();
        (*exploreRequest)->signal_failed().connect( sigc::mem_fun( this, &FileBrowserWidgetFolderItem::exploreFailedCb ) );

        if( (*exploreRequest)->getFinished() )
            kDebug() << "explore finished before signal_finished.";
    }
}

void FileBrowserWidgetFolderItem::setupUi()
{
    setExpanded( false );
    setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
}

void FileBrowserWidgetFolderItem::exploreFailedCb( GError *value )
{
    Q_UNUSED( value )
    kDebug() << "failed.";
}

// FileBrowserWidget

FileBrowserTreeWidget::FileBrowserTreeWidget( QWidget *parent )
    : QTreeWidget( parent )
    , infinoteManager( 0 )
    , clientBrowser( 0 )
    , connection( 0 )
    , rootNode( 0 )
{
    setupUi();
    setupActions();
}

FileBrowserTreeWidget::FileBrowserTreeWidget( Connection &conn, QWidget *parent )
    : QTreeWidget( parent )
    , infinoteManager( &conn.getInfinoteManager() )
    , clientBrowser(  &conn.getClientBrowser() )
    , rootNode( 0 )
{
    setConnection( &conn );
    setupUi();
    setupActions();
    createRootNodes();
}

FileBrowserTreeWidget::~FileBrowserTreeWidget()
{
    delete rootNode;
}

QList<FileBrowserWidgetItem*> FileBrowserTreeWidget::getSelectedNodes()
{
    QList<QTreeWidgetItem*> items = selectedItems();
    QList<QTreeWidgetItem*>::iterator itr;
    QList<FileBrowserWidgetItem*> nodeItems;

    for( itr = items.begin(); itr != items.end(); ++itr )
        nodeItems += dynamic_cast<FileBrowserWidgetItem*>(*itr);

    return nodeItems;
}

InfinoteManager *FileBrowserTreeWidget::getInfinoteManager() const
{
    if( connection )
        return &connection->getInfinoteManager();

    return 0;
}

Connection *FileBrowserTreeWidget::getConnection() const
{
    return connection;
}

void FileBrowserTreeWidget::unsetConnection()
{
    clear();
    setEnabled( false );
    connection = 0;
    clientBrowser = 0;
    infinoteManager = 0;
}

void FileBrowserTreeWidget::setConnection( Connection *conn )
{
    if( !conn )
        return unsetConnection();
    clear();
    connection = conn;
    clientBrowser = &conn->getClientBrowser();
    infinoteManager = &conn->getInfinoteManager();
    createRootNodes();
    setupConnectionActions();
    setEnabled( true );
}

void FileBrowserTreeWidget::slotItemExpanded( QTreeWidgetItem *item )
{
    FileBrowserWidgetFolderItem *folderItem;
    folderItem = dynamic_cast<FileBrowserWidgetFolderItem*>(item);
    folderItem->populate();
}

void FileBrowserTreeWidget::slotConnectionStatusChanged( int status )
{
    if( status == Infinity::XML_CONNECTION_OPEN )
        setEnabled( true );
    else
        setEnabled( false );
}

void FileBrowserTreeWidget::setupUi()
{
    setHeaderLabel( "Nodes" );

    if( clientBrowser )
        setEnabled( true );
    else
        setEnabled( false );
}

void FileBrowserTreeWidget::setupActions()
{
    connect( this, SIGNAL( itemExpanded( QTreeWidgetItem* ) ), this, SLOT( slotItemExpanded( QTreeWidgetItem* ) ) );
    setupConnectionActions();
}

void FileBrowserTreeWidget::setupConnectionActions()
{
    if( connection )
    {
        connect( connection, SIGNAL( statusChanged( int ) ), this, SLOT( slotConnectionStatusChanged( int ) ) );
        connection->getClientBrowser().signal_node_added().connect( sigc::mem_fun(
            this, &FileBrowserTreeWidget::nodeAddedCb ) );
        connection->getClientBrowser().signal_node_removed().connect( sigc::mem_fun(
            this, &FileBrowserTreeWidget::nodeRemovedCb ) );
    }
}

void FileBrowserTreeWidget::createRootNodes()
{
    FileBrowserWidgetFolderItem *rootNodeItem;

    rootNode = new Infinity::ClientBrowserIter;
    clientBrowser->setRootNode( *rootNode );
    rootNodeItem = new FileBrowserWidgetFolderItem( "/", *rootNode, this );
    addTopLevelItem( rootNodeItem );
}

void FileBrowserTreeWidget::nodeAddedCb( Infinity::ClientBrowserIter node )
{
    FileBrowserWidgetItem *found;
    node.setBrowser( *clientBrowser );
    Infinity::ClientBrowserIter parentItr = node;
    parentItr.parent();

    if( !(found = findNodeItem( parentItr )) )
    {
        kDebug() << "Item " << parentItr.getName() << " not found.";
    }
    else
    {
        if( node.isDirectory() )
            found->addChild( new FileBrowserWidgetFolderItem( node ) );
        else
            found->addChild( new FileBrowserWidgetNoteItem( node ) );
    }
}

void FileBrowserTreeWidget::nodeRemovedCb( Infinity::ClientBrowserIter node )
{
    FileBrowserWidgetItem *found;
    
    if( !(found = findNodeItem( node )) )
    {
        kDebug() << "Item " << node.getName() << " not found.";
    }
    else
    {
        delete found;
    }
}

FileBrowserWidgetItem *FileBrowserTreeWidget::findNodeItem( Infinity::ClientBrowserIter &itr )
{
    QList<QTreeWidgetItem*> found;
    itr.setBrowser( *clientBrowser );

    found = findItems( itr.getName(), Qt::MatchContains | Qt::MatchRecursive );

    if( found.size() == 0 )
        return 0;
    else
        return findNodeInList( itr, found );
}

FileBrowserWidgetItem *FileBrowserTreeWidget::findNodeInList( Infinity::ClientBrowserIter &iter, QList<QTreeWidgetItem*> items )
{
    QList<QTreeWidgetItem*>::iterator itr;
    if( !items.size() )
        kDebug() << "no items to find in.";

    for( itr = items.begin(); itr != items.end(); itr++ )
    {
        FileBrowserWidgetItem *item = dynamic_cast<FileBrowserWidgetItem*>(*itr);
        if( item->getNode() == iter )
        {
            return item;
        }
        else
            kDebug() << iter.getName() << " != " << item->getNode().getName();
    }

    return 0;
}


NewFolderDialog::NewFolderDialog( Infinity::ClientBrowserIter &iter, QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::NewFolderWidget )
    , parentIter( &iter )
{
    setupUi();
    setupActions();
}

void NewFolderDialog::slotCreate()
{
    emit( create( ui->nameLineEdit->text(), *parentIter ) );
}

void NewFolderDialog::setupUi()
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    setCaption( "Kobby - Create Folder" );
    setButtons( KDialog::Ok | KDialog::Cancel );
    setMainWidget( mainWidget );
}

void NewFolderDialog::setupActions()
{
    connect( this, SIGNAL( okClicked() ), this, SLOT( slotCreate() ) );
}

FileBrowserWidget::FileBrowserWidget( Connection &connection, QWidget *parent )
    : QWidget( parent )
    , ui( new Ui::FileBrowserWidget )
{
    ui->setupUi( this );
    setupUi();
    setupActions();
    ui->treeWidget->setConnection( &connection );
}

FileBrowserWidget::FileBrowserWidget( QWidget *parent )
    : QWidget( parent )
    , ui( new Ui::FileBrowserWidget )
{
    ui->setupUi( this );
    setupUi();
    setupActions();
}

FileBrowserTreeWidget &FileBrowserWidget::getTreeWidget() const
{
    return *ui->treeWidget;
}

void FileBrowserWidget::addFolder( const QString &name, Infinity::ClientBrowserIter &parentNode )
{
    Connection *conn = getTreeWidget().getConnection();

    if( !conn )
    {
        kDebug() << "No connection to add folder over.";
        return;
    }

    if( !parentNode.isExplored() )
    {
        kDebug() << "Parent node to add folder to is not explored.";
        return;
    }

    RequestProgressDialog *progressDialog = new RequestProgressDialog( "Creating folder...", this );
    progressDialog->addRequest( conn->getClientBrowser().addSubdirectory( parentNode, name.toAscii() ) );
    progressDialog->setVisible( true );
}

void FileBrowserWidget::slotNodeSelectionChanged()
{
    kDebug() << "node selection changed.";
    QList<FileBrowserWidgetItem*> items = ui->treeWidget->getSelectedNodes();
    if( items.size() > 0 )
    {
        ui->deleteButton->setEnabled( true );
        if( items.size() == 1 && items.at(0)->getNode().isDirectory() )
        {
            ui->createFolderButton->setEnabled( true );
            ui->createNoteButton->setEnabled( true );
        }
        else
        {
            ui->createFolderButton->setEnabled( false );
            ui->createNoteButton->setEnabled( false );
        }
    }
    else
    {
        ui->createFolderButton->setEnabled( false );
        ui->createNoteButton->setEnabled( false );
        ui->deleteButton->setEnabled( false );
    }
}

void FileBrowserWidget::slotNewFolderDialog()
{
    QList<FileBrowserWidgetItem*> items = getTreeWidget().getSelectedNodes();
    if( items.size() == 1 )
    {
        kDebug() << "Create folder dialog.";
        NewFolderDialog *dialog = new NewFolderDialog( items.at(0)->getNode(), this );
        connect( dialog, SIGNAL( create( const QString&, Infinity::ClientBrowserIter& ) ),
            this, SLOT( addFolder( const QString&, Infinity::ClientBrowserIter& ) ) );
        dialog->setVisible( true );
    }
    else
        kDebug() << "Cannot add folder to more than one parent.";
}

void FileBrowserWidget::slotRemoveNodes()
{
    QList<FileBrowserWidgetItem*> items = getTreeWidget().getSelectedNodes();
    QList<FileBrowserWidgetItem*>::iterator itr;
    Infinity::ClientBrowser *browser = &getTreeWidget().getConnection()->getClientBrowser();
    RequestProgressDialog *requestDialog = new RequestProgressDialog( "Removing nodes...", this );
    requestDialog->setVisible( true );

    for( itr = items.begin(); itr != items.end(); ++itr )
    {
        requestDialog->addRequest( browser->removeNode( (*itr)->getNode() ) );
    }
}

void FileBrowserWidget::setupUi()
{
    ui->createFolderButton->setIcon( KIcon( "folder-new.png" ) );
    ui->createNoteButton->setIcon( KIcon( "document-new.png" ) );
    ui->deleteButton->setIcon( KIcon( "edit-delete.png" ) );
}

void FileBrowserWidget::setupActions()
{
    connect( ui->treeWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( slotNodeSelectionChanged() ) );
    connect( ui->createFolderButton, SIGNAL( clicked() ), this, SLOT( slotNewFolderDialog() ) );
    connect( ui->deleteButton, SIGNAL( clicked() ), this, SLOT( slotRemoveNodes() ) );
}

FileBrowserDialog::FileBrowserDialog( Connection &conn, QWidget *parent )
    : KDialog( parent )
    , fileBrowserWidget( new FileBrowserWidget( conn, this ) )
{
    setupUi();
}

FileBrowserDialog::~FileBrowserDialog()
{
}

void FileBrowserDialog::setupUi()
{
    setCaption( "File Browser" );
    setMainWidget( fileBrowserWidget );
}

}
