#include <libinfinitymm/common/xmppconnection.h>

#include "connectionmanager.h"
#include "connectionmanagerlistitem.h"

#include <KDebug>

namespace Kobby
{

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
    connect( ui.addConnectionButton, SIGNAL( clicked() ), this, SLOT( slotAddConnectionDialog() ) );
    
    ui.removeConnectionButton->setIcon( KIcon( "list-remove.png" ) );
}

void ConnectionManager::addConnection( const QString hostname, unsigned int port )
{
    Infinity::XmppConnection &connection = infinoteManager->newXmppConnection( hostname, port, "greg@greghaynes.net", 0, 0 );

    addConnection( connection, hostname );
}

void ConnectionManager::addConnection( Infinity::XmppConnection &conn, const QString &hostname )
{
    ConnectionManagerListItem *connectionItem = new ConnectionManagerListItem( conn, hostname );
    ui.connectionsListWidget->addItem( connectionItem );
}

void ConnectionManager::slotAddConnectionDialog()
{
    if( addConnectionDialog )
    {
        kDebug() << "Add connection dialog already open.";
        return;
    }
    
    addConnectionDialog = new AddConnectionDialog( this );
    
    connect( addConnectionDialog, SIGNAL( addConnection( const QString, unsigned int ) ),
        this, SLOT( addConnection( const QString, unsigned int ) ) );
    connect( addConnectionDialog, SIGNAL( finished() ), this, SLOT( slotAddConnectionDialogFinished() ) );
    
    addConnectionDialog->setVisible( true );
}

void ConnectionManager::slotAddConnectionDialogFinished()
{
    addConnectionDialog = 0;
}

} // namespace Kobby
