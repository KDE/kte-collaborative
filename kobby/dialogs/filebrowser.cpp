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
    if( is_populated )
        return;
    else if( node->isExplored() )
        exploreFinishedCb();
    else
    {
        exploreRequest = new Glib::RefPtr<Infinity::ClientExploreRequest>;
        *exploreRequest = node->explore();
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

    for( res = node->child(); res; res = node->next() )
    {
        if( node->isDirectory() )
            addChild( new FileBrowserWidgetFolderItem( *node ) );
        else
            addChild( new FileBrowserWidgetNoteItem( *node ) );
    }

    setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicatorWhenChildless );
}

void FileBrowserWidgetFolderItem::exploreFailedCb( GError *value )
{
    kDebug() << "failed.";
}

// FileBrowserWidget

FileBrowserWidget::FileBrowserWidget( QWidget *parent )
    : QTreeWidget( parent )
    , infinoteManager( 0 )
    , clientBrowser( 0 )
    , connection( 0 )
    , rootNode( 0 )
{
    setupUi();
    setupActions();
}

FileBrowserWidget::FileBrowserWidget( const Connection &conn, QWidget *parent )
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

FileBrowserWidget::~FileBrowserWidget()
{
    delete rootNode;
}

void FileBrowserWidget::slotItemExpanded( QTreeWidgetItem *item )
{
    FileBrowserWidgetFolderItem *folderItem;
    folderItem = dynamic_cast<FileBrowserWidgetFolderItem*>(item);
    folderItem->populate();
}

void FileBrowserWidget::setupUi()
{
    setHeaderLabel( "Nodes" );

    if( clientBrowser )
        setEnabled( true );
    else
        setEnabled( false );
}

void FileBrowserWidget::setupActions()
{
    connect( this, SIGNAL( itemExpanded( QTreeWidgetItem* ) ), this, SLOT( slotItemExpanded( QTreeWidgetItem* ) ) );
}

void FileBrowserWidget::createRootNodes()
{
    FileBrowserWidgetFolderItem *rootNodeItem;

    rootNode = new Infinity::ClientBrowserIter;
    clientBrowser->setRootNode( *rootNode );
    rootNodeItem = new FileBrowserWidgetFolderItem( "/", *rootNode, this );
    addTopLevelItem( rootNodeItem );
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
