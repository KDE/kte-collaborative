#include <libinfinitymm/client/clientbrowseriter.h>

#include "browsermodel.h"
#include "browseritem.h"

#include <libqinfinitymm/connection.h>

#include <KDebug>

namespace Kobby
{

BrowserModel::BrowserModel( QObject *parent )
    : QInfinity::BrowserModel( parent )
{
}

void BrowserModel::addConnection( QInfinity::Connection &connection )
{
    QInfinity::BrowserConnectionItem *item = new BrowserConnectionItem( connection, this );
    addConnectionItem( *item );
}

void BrowserModel::addNode( const Infinity::ClientBrowserIter &iter )
{
    Infinity::ClientBrowserIter t_iter = iter;
    QInfinity::BrowserNodeItem *nodeItem;

    if( t_iter.isDirectory() )
        nodeItem = new BrowserFolderItem( iter, this );
    else
        nodeItem = new BrowserNoteItem( iter, this );

    addNodeItem( *nodeItem );
}

}

