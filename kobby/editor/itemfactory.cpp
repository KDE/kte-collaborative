#include "itemfactory.h"
#include "connection.h"

#include <libqinfinity/browseriter.h>

#include <KIcon>

namespace Kobby
{

ConnectionItem::ConnectionItem( QInfinity::XmlConnection &conn,
    const KIcon &icon,
    const QString &text )
    : QInfinity::ConnectionItem( conn, icon, text )
    , m_conn( 0 )
{
}

ConnectionItem::~ConnectionItem()
{
    if( m_conn )
        delete m_conn;
}

void ConnectionItem::setConnection( Connection *conn )
{
    m_conn = conn;
}

ItemFactory::ItemFactory( QObject *parent )
    : QInfinity::BrowserItemFactory( parent )
{
}

QInfinity::NodeItem *ItemFactory::createRootNodeItem( const QInfinity::BrowserIter &iter )
{
    QInfinity::NodeItem *item;
    item = new QInfinity::NodeItem( iter, KIcon("folder.png"), "/" );
    return item;
}

QInfinity::NodeItem *ItemFactory::createNodeItem( const QInfinity::BrowserIter &iter )
{
    QInfinity::BrowserIter localItr = iter;

    QInfinity::NodeItem *item;
    if( localItr.isDirectory() )
        item = new QInfinity::NodeItem( localItr, KIcon("folder.png") );
    else
        item = new QInfinity::NodeItem( localItr, KIcon("document.png") );
    return item;
}

QInfinity::ConnectionItem *ItemFactory::createConnectionItem( QInfinity::XmlConnection &conn,
    const QString &name )
{
    return new Kobby::ConnectionItem( conn, KIcon("network-connect.png"), name );
}

}

