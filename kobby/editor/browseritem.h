#ifndef KOBBY_BROWSERITEM_H
#define KOBBY_BROWSERITEM_H

#include <libqinfinitymm/browseritem.h>

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

class BrowserConnectionItem
    : public QInfinity::BrowserConnectionItem
{

    public:
        BrowserConnectionItem( QInfinity::Connection &connection,
            QObject *parent = 0 );

    protected:
        void onConnected();
        void onConnecting();
        void onDisconnected();

};

class BrowserFolderItem
    : public QInfinity::BrowserFolderItem
{

    public:
        BrowserFolderItem( const Infinity::ClientBrowserIter &iter,
            QObject *parent = 0 );
    
};

class BrowserNoteItem
    : public QInfinity::BrowserNoteItem
{

    public:
        BrowserNoteItem( const Infinity::ClientBrowserIter &iter,
            QObject *parent = 0 );

};

}

#endif

