#include <libinfinitymm/init.h>
#include <libinfinitymm/common/xmppconnection.h>
#include <libinfinitymm/common/standaloneio.h>

#include <KPushButton>

#include "joinsessiondialog.h"

namespace Kobby
{

JoinSessionDialog::JoinSessionDialog( QWidget *parent )
    : KDialog( parent )
{
    QWidget *widget = new QWidget( this );
    ui.setupUi( widget );
    setMainWidget( widget );
    
    setButtons( KDialog::Try | KDialog::Close );
    
    button( KDialog::Try )->setEnabled( false );
    
    setupActions();
}

void JoinSessionDialog::slotLocationChanged( const QString &text )
{
    Q_UNUSED( text )
    
    // Try should be disabled
    if( ui.hostnameLineEdit->text() == "" || ui.portLineEdit->text() == "" )
    {
        if( button( KDialog::Try )->isEnabled() )
            button( KDialog::Try )->setEnabled( false );
    }
    else // Try should be enabled
    {
        if( !button( KDialog::Try )->isEnabled() )
            button( KDialog::Try )->setEnabled( true );
    }
}

void JoinSessionDialog::tryConnecting()
{
    QString connectingMsg;
    
    ui.hostnameLineEdit->setEnabled( false );
    ui.portLineEdit->setEnabled( false );
    button( KDialog::Try )->setEnabled( false );
    
    connectingMsg +=  i18n("Connecting to ");
    connectingMsg += ui.hostnameLineEdit->text();
    connectingMsg += ":";
    connectingMsg += ui.portLineEdit->text();
    connectingMsg += "\n";
    
    ui.statusTextView->setEnabled( true );
    ui.statusTextView->insertPlainText( connectingMsg );
    
    Infinity::init();
    
    Infinity::StandaloneIo io;
    Infinity::IpAddress address(Infinity::IP_ADDRESS_IPV4);
    Infinity::TcpConnection tcpConnection(io, address, 5223);
}

void JoinSessionDialog::setupActions()
{
    connect( this, SIGNAL( tryClicked() ), this, SLOT( tryConnecting() ) );
    connect( ui.hostnameLineEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotLocationChanged( const QString& ) ) );
    connect( ui.portLineEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotLocationChanged( const QString& ) ) );
}

} // namespace Kobby
