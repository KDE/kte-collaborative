#include <libinfinitymm/common/xmppconnection.h>

#include "connectionmanager.h"

#include <kobby/infinote/infinotemanager.h>

#include <KDebug>

#include "kobby/ui_connectionmanager.h"

namespace Kobby
{

ConnectionManager::ConnectionManager( InfinoteManager &manager, QWidget *parent )
    : KDialog( parent )
    , addConnectionDialog( 0 )
    , fileBrowserDialog( 0 )
    , infinoteManager( &manager )
    , ui( new Ui::ConnectionManager )
{
    QWidget *widget = new QWidget( this );
    ui->setupUi( widget );
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
    ui->addConnectionButton->setIcon( KIcon( "list-add.png" ) );
    connect( ui->addConnectionButton, SIGNAL(clicked()), this, SLOT(slotAddConnectionDialog()) );

    ui->filesButton->setIcon( KIcon( "folder.png" ) );
    connect( ui->filesButton, SIGNAL(clicked()), this, SLOT(slotFileBrowser()) );
    
    ui->removeConnectionButton->setIcon( KIcon( "list-remove.png" ) );
    connect( ui->removeConnectionButton, SIGNAL(clicked()), this, SLOT(slotRemoveSelectedItems()) );

    connect( ui->connectionsListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()) );
}

void ConnectionManager::addConnection( const QString name, const QString hostname, unsigned int port )
{
    ConnectionListWidgetItem *listItem;

    listItem = new ConnectionListWidgetItem( infinoteManager->connectToHost( name, hostname, port ) );
    ui->connectionsListWidget->addItem( listItem );
}

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

void ConnectionManager::slotFileBrowser()
{
    if( fileBrowserDialog )
    {
        kDebug() << "File browser dialog already open.";
        return;
    }

    ConnectionListWidgetItem *connection;
    
    connection = dynamic_cast<ConnectionListWidgetItem*>( ui->connectionsListWidget->currentItem() );

    fileBrowserDialog = new FileBrowserDialog( *this->infinoteManager, connection->getConnection(), this );
    fileBrowserDialog->setVisible( true );
}

void ConnectionManager::slotSelectionChanged()
{
    if( !(ui->removeConnectionButton->isEnabled()) ) {
        ui->removeConnectionButton->setEnabled( true );
        ui->filesButton->setEnabled( true );
    }
}

void ConnectionManager::slotRemoveSelectedItems()
{
    QList<QListWidgetItem*> items;
    QList<QListWidgetItem*>::Iterator itr;

    items = ui->connectionsListWidget->selectedItems();

    for( itr = items.begin(); itr != items.end(); itr++ )
        delete *itr;

    items = ui->connectionsListWidget->selectedItems();
    if( items.count() == 0 )
        ui->removeConnectionButton->setEnabled( false );
}

} // namespace Kobby
