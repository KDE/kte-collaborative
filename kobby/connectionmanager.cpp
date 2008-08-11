#include <libinfinitymm/common/xmppconnection.h>

#include "connectionmanager.h"
#include "connectionmanagerlistitem.h"

#include <KDebug>

namespace Kobby
{

ConnectionListWidgetItem::ConnectionListWidgetItem( Connection &conn, QListWidget *parent )
    : QListWidgetItem( parent )
    , connection( &conn )
    , has_connected( false )
{
    setDisplay();

    connection->getTcpConnection().property_status().signal_changed().connect( sigc::mem_fun( this, &ConnectionListWidgetItem::setDisplay ) );
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

ConnectionManager::ConnectionManager( InfinoteManager &manager, QWidget *parent )
    : KDialog( parent )
    , addConnectionDialog( 0 )
    , infinoteManager( &manager )
{
    QWidget *widget = new QWidget( this );
    ui.setupUi( widget );
    setMainWidget( widget );
    
    setCaption( i18n( "Kobby Connection Manager" ) );
    
    setButtons( KDialog::Close );
    
    setupActions();
}

ConnectionManager::~ConnectionManager()
{
    if( addConnectionDialog )
        addConnectionDialog->close();
}

void ConnectionManager::setupActions()
{
    ui.addConnectionButton->setIcon( KIcon( "list-add.png" ) );
    connect( ui.addConnectionButton, SIGNAL(clicked()), this, SLOT(slotAddConnectionDialog()) );
    
    ui.removeConnectionButton->setIcon( KIcon( "list-remove.png" ) );
    connect( ui.removeConnectionButton, SIGNAL(clicked()), this, SLOT(slotRemoveSelectedItems()) );

    connect( ui.connectionsListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()) );
}

void ConnectionManager::addConnection( const QString name, const QString hostname, unsigned int port )
{
    ConnectionListWidgetItem *listItem;

    listItem = new ConnectionListWidgetItem( infinoteManager->connectToHost( name, hostname, port ) );
    ui.connectionsListWidget->addItem( listItem );
}

/*
void ConnectionManager::addConnection( Infinity::XmppConnection &conn, const QString &hostname )
{
    ConnectionManagerListItem *connectionItem = new ConnectionManagerListItem( conn, hostname );
    ui.connectionsListWidget->addItem( connectionItem );
}
*/

void ConnectionManager::slotAddConnectionDialog()
{
    if( addConnectionDialog )
    {
        kDebug() << "Add connection dialog already open.";
        return;
    }
    
    addConnectionDialog = new AddConnectionDialog( this );
    
    connect( addConnectionDialog, SIGNAL( addConnection( const QString, const QString, unsigned int ) ),
        this, SLOT( addConnection( const QString, const QString, unsigned int ) ) );
    connect( addConnectionDialog, SIGNAL( finished() ), this, SLOT( slotAddConnectionDialogFinished() ) );
    
    addConnectionDialog->setVisible( true );
}

void ConnectionManager::slotAddConnectionDialogFinished()
{
    addConnectionDialog = 0;
}

void ConnectionManager::slotSelectionChanged()
{
    if( !(ui.removeConnectionButton->isEnabled()) )
        ui.removeConnectionButton->setEnabled( true );
}

void ConnectionManager::slotRemoveSelectedItems()
{
    QList<QListWidgetItem*> items;
    QList<QListWidgetItem*>::Iterator itr;

    items = ui.connectionsListWidget->selectedItems();

    for( itr = items.begin(); itr != items.end(); itr++ )
        delete *itr;

    items = ui.connectionsListWidget->selectedItems();
    if( items.count() == 0 )
        ui.removeConnectionButton->setEnabled( false );
}

} // namespace Kobby
