#ifndef KOBBY_ITEMFACTORY_H
#define KOBBY_ITEMFACTORY_H

#include <libqinfinity/browseritemfactory.h>

class KIcon;
class QObject;
class QString;

namespace QInfinity
{
    class XmlConnection;
    class Browser;
    class BrowserIter;
}

namespace Kobby
{

class Connection;

/**
 * Subclass so we can store connections with their item
 */
class ConnectionItem
    : public QInfinity::ConnectionItem
{

    public:
        ConnectionItem( QInfinity::XmlConnection &conn,
            QInfinity::Browser &browser,
            const KIcon &icon,
            const QString &text );
        ~ConnectionItem();

        void setConnection( Connection *conn );

    private:
        Connection *m_conn;

};



class ItemFactory
    : public QInfinity::BrowserItemFactory
{

    public:
        ItemFactory( QObject *parent = 0 );

        QInfinity::NodeItem *createRootNodeItem( const QInfinity::BrowserIter &iter );
        QInfinity::NodeItem *createNodeItem( const QInfinity::BrowserIter &iter );
        QInfinity::ConnectionItem *createConnectionItem( QInfinity::XmlConnection &conn,
            QInfinity::Browser &browser,
            const QString &name );

};

}

#endif

