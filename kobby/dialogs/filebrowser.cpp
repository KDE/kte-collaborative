#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>
#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientbrowseriter.h>
#include <libinfinitymm/client/clientexplorerequest.h>

#include <qinfinitymm/qtio.h>

#include "filebrowser.h"

#include <kobby/infinote/connection.h>
#include <kobby/infinote/infinotemanager.h>

#include <KDebug>
#include <KIcon>
#include <KPushButton>

#include <glib/gerror.h>

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
        exploreFinishedCb();
    else
    {
        exploreRequest = new Glib::RefPtr<Infinity::ClientExploreRequest>;
        *exploreRequest = getNode().explore();
        (*exploreRequest)->signal_finished().connect( sigc::mem_fun( this, &FileBrowserWidgetFolderItem::exploreFinishedCb ) );
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

void FileBrowserWidgetFolderItem::exploreFinishedCb()
{
    kDebug() << "finished.";

    is_populated = true;
    bool res;
    Infinity::ClientBrowserIter itr;
    itr = getNode();

    for( res = itr.child(); res; res = itr.next() )
    {
        if( itr.isDirectory() )
            addChild( new FileBrowserWidgetFolderItem( itr ) );
        else
            addChild( new FileBrowserWidgetNoteItem( itr ) );
    }

    setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicatorWhenChildless );
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

FileBrowserTreeWidget::FileBrowserTreeWidget( const Connection &conn, QWidget *parent )
    : QTreeWidget( parent )
    , infinoteManager( &conn.getInfinoteManager() )
    , clientBrowser(  &conn.getClientBrowser() )
    , connection( &conn )
    , rootNode( 0 )
{
    setupUi();
    setupActions();
    createRootNodes();
}

FileBrowserTreeWidget::~FileBrowserTreeWidget()
{
    delete rootNode;
}

void FileBrowserTreeWidget::unsetConnection()
{
    clear();
    setEnabled( false );
    connection = 0;
    clientBrowser = 0;
    infinoteManager = 0;
}

void FileBrowserTreeWidget::setConnection( const Connection *conn )
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

void FileBrowserTreeWidget::setConnection( Connection *conn )
{
    setConnection( (const Connection*) conn );
}

void FileBrowserTreeWidget::slotItemExpanded( QTreeWidgetItem *item )
{
    FileBrowserWidgetFolderItem *folderItem;
    folderItem = dynamic_cast<FileBrowserWidgetFolderItem*>(item);
    folderItem->populate();
}

void FileBrowserTreeWidget::slotItemSelectionChanged()
{
    QList<QTreeWidgetItem*> items = selectedItems();
    QList<QTreeWidgetItem*>::iterator itr;
    QList<FileBrowserWidgetItem*> nodeItems;

    for( itr = items.begin(); itr != items.end(); ++itr )
        nodeItems += dynamic_cast<FileBrowserWidgetItem*>(*itr);

    emit( nodeSelectionChanged( nodeItems ) );
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
    connect( this, SIGNAL( itemSelectionChanged() ), this, SLOT( slotItemSelectionChanged() ) );
}

void FileBrowserTreeWidget::setupConnectionActions()
{
    if( connection )
        connect( connection, SIGNAL( statusChanged( int ) ), this, SLOT( slotConnectionStatusChanged( int ) ) );
}

void FileBrowserTreeWidget::createRootNodes()
{
    FileBrowserWidgetFolderItem *rootNodeItem;

    rootNode = new Infinity::ClientBrowserIter;
    clientBrowser->setRootNode( *rootNode );
    rootNodeItem = new FileBrowserWidgetFolderItem( "/", *rootNode, this );
    addTopLevelItem( rootNodeItem );
}

FileBrowserWidget::FileBrowserWidget( const Connection &connection, QWidget *parent )
    : QWidget( parent )
    , ui( new Ui::FileBrowserWidget )
{
    ui->setupUi( this );
    ui->treeWidget->setConnection( &connection );
}

FileBrowserWidget::FileBrowserWidget( QWidget *parent )
    : QWidget( parent )
    , ui( new Ui::FileBrowserWidget )
{
    ui->setupUi( this );
}

FileBrowserTreeWidget &FileBrowserWidget::getTreeWidget() const
{
    return *ui->treeWidget;
}

FileBrowserDialog::FileBrowserDialog( const Connection &conn, QWidget *parent )
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
