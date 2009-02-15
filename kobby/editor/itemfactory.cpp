#include "itemfactory.h"

#include <libqinfinity/browseriter.h>

#include <KIcon>

namespace Kobby
{

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
    return new QInfinity::ConnectionItem( conn, KIcon("network-connect.png"), name );
}

}

