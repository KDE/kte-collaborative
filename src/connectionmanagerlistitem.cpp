#include <libinfinitymm/common/xmppconnection.h>

#include "connectionmanagerlistitem.h"

namespace Kobby
{

ConnectionManagerListItem::ConnectionManagerListItem( Infinity::XmppConnection &conn,
    const QString &hostname, QListWidget *parent )
    : QListWidgetItem( hostname, parent )
    , connection( &conn )
{
    setText( hostname + QString(":") +  QString::number(conn.property_tcp_connection().get_value()->getRemotePort()) + "\nConnecting..." );
}

void ConnectionManagerListItem::onConnectionStatusChanged()
{
    
}

}
