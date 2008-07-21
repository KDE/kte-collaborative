#include <libinfinitymm/common/xmppconnection.h>

#include "connectionmanagerlistitem.h"

namespace Kobby
{

ConnectionManagerListItem::ConnectionManagerListItem( Infinity::XmppConnection &conn,
    const QString &hostname, QListWidget *parent )
    : QListWidgetItem( hostname, parent )
    , connection( &conn )
{
    setText( hostname + "\n text" );
}

void ConnectionManagerListItem::onConnectionStatusChanged()
{
    
}

}
