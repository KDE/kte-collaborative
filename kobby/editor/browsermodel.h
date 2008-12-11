#ifndef KOBBY_BROWSERMODEL_H
#define KOBBY_BROWSERMODEL_H

#include <libqinfinitymm/browsermodel.h>

namespace Infinity
{
    class ClientBrowserIter;
}

namespace QInfinity
{
    class Connection;
}

namespace Kobby
{

class BrowserModel
    : public QInfinity::BrowserModel
{
    Q_OBJECT

    public:
        BrowserModel( QObject *parent = 0 );

    protected Q_SLOTS:
        /**
         * We are overriding this to make use of the KDE icon system.
         */
        void addConnection( QInfinity::Connection &connection );
        /**
         * We are overriding this to make use of the KDE icon system.
         */
        void addNode( const Infinity::ClientBrowserIter &iter );

};

}

#endif

