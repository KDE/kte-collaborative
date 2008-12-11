#include <libinfinitymm/client/clientbrowseriter.h>
#include "browseritem.h"

#include <libqinfinitymm/connection.h>

#include <KIcon>

namespace Kobby
{

BrowserConnectionItem::BrowserConnectionItem( QInfinity::Connection &connection,
    QObject *parent )
    : QInfinity::BrowserConnectionItem( connection, parent )
{
}

void BrowserConnectionItem::onConnected()
{
    setIcon( KIcon( "network-connect.png" ) );
}

void BrowserConnectionItem::onConnecting()
{
    setIcon( KIcon( "network-disconnect.png" ) );
}

void BrowserConnectionItem::onDisconnected()
{
    setIcon( KIcon( "network-disconnect.png" ) );
}

BrowserFolderItem::BrowserFolderItem( const Infinity::ClientBrowserIter &iter,
    QObject *parent )
    : QInfinity::BrowserFolderItem( iter, parent )
{
}

BrowserNoteItem::BrowserNoteItem( const Infinity::ClientBrowserIter &iter,
    QObject *parent )
    : QInfinity::BrowserNoteItem( iter, parent )
{
}

}

