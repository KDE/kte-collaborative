#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>

#include "connectionmanager.h"

#include "kobby/infinote/infinotemanager.h"

#include <KIcon>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace Kobby
{

ConnectionListWidgetItem::ConnectionListWidgetItem( Connection &conn, QListWidget *parent )
    : QObject( parent)
    , QListWidgetItem( parent, QListWidgetItem::UserType )
    , connection( &conn )
    , has_connected( false )
{
    setDisplay();

    connect( connection, SIGNAL( statusChanged( int ) ), this, SLOT( slotStatusChanged( int ) ) );
}

void ConnectionListWidgetItem::setDisplay()
{
    QString statusLine;

    switch( connection->getTcpConnection().property_status() )
    {
        case Infinity::TCP_CONNECTION_CONNECTING:
            setIcon( KIcon( "network-disconnect.png" ) );
            statusLine = "Connecting...";
            break;
        case Infinity::TCP_CONNECTION_CONNECTED:
            has_connected = true;
            setIcon( KIcon( "network-connect.png" ) );
            statusLine = "Connected.";
            break;
        case Infinity::TCP_CONNECTION_CLOSED:
            if( !has_connected )
                statusLine = "Could not connect to server.";
            else
                statusLine = "Closed.";
            setIcon( KIcon( "network-disconnect.png" ) );
    }

    setText( connection->getName() + "\n" 
        + statusLine
    );
}

Connection &ConnectionListWidgetItem::getConnection()
{
    return *connection;
}

void ConnectionListWidgetItem::slotStatusChanged( int status )
{
    Q_UNUSED( status )
    setDisplay();
}

ConnectionListWidget::ConnectionListWidget( InfinoteManager &infinoteManager, QWidget *parent )
    : QListWidget( parent )
    , infinoteManager( &infinoteManager )
{
}

void ConnectionListWidget::addConnections( QList<Connection*> &connections )
{
    QList<Connection*>::iterator itr;

    for( itr = connections.begin(); itr != connections.end(); ++itr )
        addItem( new ConnectionListWidgetItem( **itr ) );
}

ConnectionManagerWidget::ConnectionManagerWidget( InfinoteManager &manager,
    QWidget *parent )
    : QWidget( parent )
    , infinoteManager( &manager )
    , connectionListWidget( new ConnectionListWidget( manager, this ) )
{
    setupUi();
}

void ConnectionManagerWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    addButton = new QPushButton( KIcon( "list-add.png" ), "Add", this );
    removeButton = new QPushButton( KIcon( "list-remove.png" ), "Remove", this );

    buttonLayout->addWidget( addButton );
    buttonLayout->addWidget( removeButton );
    buttonLayout->addStretch();

    mainLayout->addWidget( connectionListWidget );
    mainLayout->addLayout( buttonLayout );

    setLayout( mainLayout );

    adjustSize();
}

ConnectionManagerDialog::ConnectionManagerDialog( InfinoteManager &manager, QWidget *parent )
    : KDialog( parent )
    , infinoteManager( &manager )
{
    ConnectionManagerWidget *managerWidget = new ConnectionManagerWidget( manager, this );
    setMainWidget( managerWidget );

    setCaption( "Connection Manager" );
    setButtons( KDialog::Close );

    adjustSize();
}

}
