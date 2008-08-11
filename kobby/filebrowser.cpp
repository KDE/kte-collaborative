#include "filebrowser.h"

#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/connectionmanager.h>
#include <libinfinitymm/client/clientbrowser.h>

#include <KIcon>

namespace Kobby
{

FileBrowserWidgetItem::FileBrowserWidgetItem( QStringList &strings, int type )
    : QTreeWidgetItem( strings, type )
{
    if( type == Folder )
        setIcon( kIcon( "places/folder.png" ) );
    else
        setIcon( kIcon( "mimitypes/text-plain.png" ) );
}

FileBrowserWidget::FileBrowserWidget( InfinoteManager &manager, Infinity::XmppConnection &conn, QWidget *parent )
    : QTreeWidget( parent )
    , infinoteManager( &manager )
    , xmppConnection( &conn )
    , connectionManager( new Infinity::ConnectionManager() )
    , clientBrowser( new Infinity::ClientBrowser( infinoteManager->getIo(), xmppConnection, connectionManager ) )
{
}

}
