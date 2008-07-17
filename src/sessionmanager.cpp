#include "sessionmanager.h"

#include <KDebug>

namespace Kobby
{

SessionManager::SessionManager( QWidget *parent )
    : KDialog( parent )
    , joinSessionDialog( 0 )
{
    QWidget *widget = new QWidget( this );
    ui.setupUi( widget );
    setMainWidget( widget );
    
    setCaption( i18n("Kobby Session Manager") );
    
    setButtons( KDialog::Close );
    
    setupActions();
}

SessionManager::~SessionManager()
{
    if( joinSessionDialog )
        joinSessionDialog->close();
}

void SessionManager::setupActions()
{
    connect( ui.sessionJoinButton, SIGNAL( clicked() ), this, SLOT( slotJoinSession() ) );
}

void SessionManager::slotJoinSession()
{
    if( joinSessionDialog )
    {
        kDebug() << "Session dialog already open.";
        return;
    }
    
    joinSessionDialog = new JoinSessionDialog( this );
    connect( joinSessionDialog, SIGNAL( finished() ), this, SLOT( slotJoinSessionFinished() ) );
    joinSessionDialog->setVisible( true );
}

void SessionManager::slotJoinSessionFinished()
{
    joinSessionDialog = 0;
}

} // namespace Kobby
