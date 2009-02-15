#ifndef KOBBY_ITEMFACTORY_H
#define KOBBY_ITEMFACTORY_H

#include <libqinfinity/browseritemfactory.h>

class QObject;
class QString;

namespace QInfinity
{
    class XmlConnection;
    class BrowserIter;
}

namespace Kobby
{

class ItemFactory
    : public QInfinity::BrowserItemFactory
{

    public:
        ItemFactory( QObject *parent = 0 );

        QInfinity::NodeItem *createRootNodeItem( const QInfinity::BrowserIter &iter );
        QInfinity::NodeItem *createNodeItem( const QInfinity::BrowserIter &iter );
        QInfinity::ConnectionItem *createConnectionItem( QInfinity::XmlConnection &conn,
            const QString &name );

};

}

#endif

