#include <libinfinitymm/init.h>
#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/standaloneio.h>

#include <KPushButton>

#include "addconnectiondialog.h"

namespace Kobby
{

AddConnectionDialog::AddConnectionDialog( QWidget *parent )
    : KDialog( parent )
{
    QWidget *widget = new QWidget( this );
    ui.setupUi( widget );
    setMainWidget( widget );
    
    setButtons( KDialog::Ok | KDialog::Close );
    
    button( KDialog::Ok )->setEnabled( false );
    
    setupActions();
}

void AddConnectionDialog::slotLocationChanged( const QString &text )
{
    Q_UNUSED( text )
    
    // Try should be disabled
    if( ui.hostnameLineEdit->text() == "" || ui.portLineEdit->text() == "" )
    {
        if( button( KDialog::Ok )->isEnabled() )
            button( KDialog::Ok )->setEnabled( false );
    }
    else // Try should be enabled
    {
        if( !button( KDialog::Ok )->isEnabled() )
            button( KDialog::Ok )->setEnabled( true );
    }
}

void AddConnectionDialog::tryConnecting()
{
    QString connectingMsg;
    
    ui.hostnameLineEdit->setEnabled( false );
    ui.portLineEdit->setEnabled( false );
    button( KDialog::Ok )->setEnabled( false );
    
    connectingMsg +=  i18n("Connecting to ");
    connectingMsg += ui.hostnameLineEdit->text();
    connectingMsg += ":";
    connectingMsg += ui.portLineEdit->text();
    connectingMsg += "\n";
    
    ui.statusTextView->setEnabled( true );
    ui.statusTextView->insertPlainText( connectingMsg );
    
    emit( addConnection( ui.hostnameLineEdit->text(), ui.portLineEdit->text().toUInt() ) );
    
}

void AddConnectionDialog::setupActions()
{
    connect( this, SIGNAL( okClicked() ), this, SLOT( tryConnecting() ) );
    connect( ui.hostnameLineEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotLocationChanged( const QString& ) ) );
    connect( ui.portLineEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotLocationChanged( const QString& ) ) );
}

} // namespace Kobby
