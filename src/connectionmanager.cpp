#include "connectionmanager.h"

#include <KDebug>

namespace Kobby
{

ConnectionManager::ConnectionManager( QWidget *parent )
    : KDialog( parent )
    , addConnectionDialog( 0 )
{
    QWidget *widget = new QWidget( this );
    ui.setupUi( widget );
    setMainWidget( widget );
    
    setCaption( i18n("Kobby Connection Manager") );
    
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
    connect( ui.sessionJoinButton, SIGNAL( clicked() ), this, SLOT( slotAddConnection() ) );
}

void ConnectionManager::slotAddConnection()
{
    if( addConnectionDialog )
    {
        kDebug() << "Add connection dialog already open.";
        return;
    }
    
    addConnectionDialog = new AddConnectionDialog( this );
    connect( addConnectionDialog, SIGNAL( finished() ), this, SLOT( slotAddConnectionFinished() ) );
    addConnectionDialog->setVisible( true );
}

void ConnectionManager::slotAddConnectionFinished()
{
    addConnectionDialog = 0;
}

} // namespace Kobby
