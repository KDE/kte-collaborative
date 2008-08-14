#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>
#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientbrowseriter.h>
#include <libinfinitymm/client/clientexplorerequest.h>

#include <qinfinitymm/qtio.h>

#include "filebrowser.h"

#include <KDebug>
#include <KIcon>
#include <KPushButton>

namespace Kobby
{

FileBrowserWidgetItem::FileBrowserWidgetItem( QStringList &strings, int type, Infinity::ClientBrowserIter &iter, QTreeWidget *parent )
    : QObject( parent ) 
    , QTreeWidgetItem( parent, strings, type )
    , node( new Infinity::ClientBrowserIter() )
{
    node->operator=( iter );
    setItemIcon();

    populateChildren();
}

FileBrowserWidgetItem::FileBrowserWidgetItem( const Infinity::ClientBrowserIter &iter, int type, QTreeWidget *parent )
    : QObject( parent )
    , QTreeWidgetItem( parent, type )
    , node( new Infinity::ClientBrowserIter() )
{
    node->operator=( iter );
    setItemIcon();

    setText( 0, node->getName() );

    populateChildren();
}

FileBrowserWidgetItem::~FileBrowserWidgetItem()
{
    delete node;
}

void FileBrowserWidgetItem::populateChildren()
{
    kDebug() << "Adding children";
    exploreRequest = node->explore();
    exploreRequest->signal_finished().connect( sigc::mem_fun( this, &FileBrowserWidgetItem::exploreFinishedCb ) );
    kDebug() << "...";
}

void FileBrowserWidgetItem::exploreFinishedCb()
{
    kDebug() << "finished.\n";

    bool res;
    QString name;
    QStringList nameList;

    for( res = node->child(); res; res = node->next() )
    {
        name = node->getName();
        nameList += name;
        addChild( new FileBrowserWidgetItem( nameList, type(), *node ) );
        nameList.clear();
    }
}

void FileBrowserWidgetItem::setItemIcon()
{
    if( type() == Folder )
        setIcon( 0, KIcon( "folder.png" ) );
    else
        setIcon( 0, KIcon( "text-plain.png" ) );
}

FileBrowserDialog::FileBrowserDialog( InfinoteManager &manager, Connection &conn, QWidget *parent )
    : KDialog( parent )
    , nodeTreeWidget( new QTreeWidget( this ) )
    , infinoteManager( &manager )
    , connection( &conn )
    , rootNode( new Infinity::ClientBrowserIter() )
{
    setButtons( KDialog::Ok | KDialog::User1 );
    button( KDialog::User1 )->setText( "Create" );
    button( KDialog::User1 )->setIcon( KIcon( "folder-new.png" ) );

    nodeTreeWidget->setHeaderLabel( "Files" );
    setMainWidget( nodeTreeWidget );

    addRootNodes();
}

FileBrowserDialog::~FileBrowserDialog()
{
}

void FileBrowserDialog::addRootNodes()
{
    connection->getClientBrowser().rootNode( *rootNode );
    exploreRequest = rootNode->explore();
    exploreRequest->signal_finished().connect( sigc::mem_fun( this, &FileBrowserDialog::exploreFinishedCb ) );
}

void FileBrowserDialog::exploreFinishedCb()
{
    Infinity::ClientBrowserIter itr;
    bool res;

    itr = *rootNode;

    for( res = itr.child(); res; res = itr.next() )
    {
        nodeTreeWidget->addTopLevelItem( new FileBrowserWidgetItem( itr, FileBrowserWidgetItem::Folder, nodeTreeWidget ) );
    }
}

}
