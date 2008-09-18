#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>

#include "connectionmanager.h"

#include "kobby/infinote/infinotemanager.h"
#include "kobby/infinote/connection.h"

#include <KLocale>
#include <KDebug>
#include <KIcon>
#include <KPushButton>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>

#include "kobby/ui_addconnectiondialog.h"
#include "kobby/ui_connectioneditor.h"

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

    switch( connection->getStatus() )
    {
        case Infinity::XML_CONNECTION_OPENING:
            setIcon( KIcon( "network-disconnect.png" ) );
            statusLine = i18n("Connecting...");
            break;
        case Infinity::XML_CONNECTION_OPEN:
            has_connected = true;
            setIcon( KIcon( "network-connect.png" ) );
            statusLine = i18n("Connected.");
            break;
        case Infinity::XML_CONNECTION_CLOSED:
            if( !has_connected )
                statusLine = i18n("Could not connect to server.");
            else
                statusLine = i18n("Closed.");
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

// ConnectionListWidget

ConnectionListWidget::ConnectionListWidget( InfinoteManager &manager, QWidget *parent )
    : QListWidget( parent )
    , infinoteManager( &manager )
{
    addConnections( infinoteManager->getConnections() );
    setupUi();
    setupActions();
}

void ConnectionListWidget::addConnection( Connection &connection )
{
    addItem( new ConnectionListWidgetItem( connection ) );
}

void ConnectionListWidget::removeConnection( Connection &connection )
{
    QList<QListWidgetItem*> found = findItems( connection.getName(), Qt::MatchContains );
    QList<QListWidgetItem*>::iterator itr;

    if( !found.size() )
        kDebug() << "could not find " << connection.getName();

    for( itr = found.begin(); itr != found.end(); ++itr )
    {
        ConnectionListWidgetItem *item = dynamic_cast<ConnectionListWidgetItem*>(*itr);
        if( item->getConnection() == connection )
        {
            kDebug() << "removing " << item->getConnection().getName();
            delete item;
        }
    }
}

void ConnectionListWidget::addConnections( QList<Connection*> &connections )
{
    QList<Connection*>::const_iterator itr;

    for( itr = connections.begin(); itr != connections.end(); ++itr )
        addItem( new ConnectionListWidgetItem( **itr ) );
}

void ConnectionListWidget::slotSelectionChanged()
{
    ConnectionListWidgetItem *selected = dynamic_cast<ConnectionListWidgetItem*>(currentItem());
    if( selected )
        emit( connectionSelectionChanged( &selected->getConnection() ) );
    else
        emit( connectionSelectionChanged( 0 ) );
}

void ConnectionListWidget::setupUi()
{
    setSelectionMode( QAbstractItemView::SingleSelection );
}

void ConnectionListWidget::setupActions()
{
    connect( infinoteManager, SIGNAL( connectionAdded( Connection & ) ),
        this, SLOT( addConnection( Connection & ) ) );
    connect( infinoteManager, SIGNAL( connectionRemoved( Connection & ) ),
        this, SLOT( removeConnection( Connection & ) ) );
    connect( this, SIGNAL( itemSelectionChanged() ), this, SLOT( slotSelectionChanged() ) );
}

// ConnectionEditorWidget
ConnectionEditorWidget::ConnectionEditorWidget( QWidget *parent )
    : QWidget( parent )
    , connection( 0 )
    , ui( new Ui::ConnectionEditor )
    , is_editable( true )
{
    ui->setupUi( this );
    setEditable( false );
}

ConnectionEditorWidget::ConnectionEditorWidget( Connection &conn, QWidget *parent )
    : QWidget( parent )
    , connection( &conn )
    , ui( new Ui::ConnectionEditor )
    , is_editable( true )
{
    ui->setupUi( this );
    setEditable( true );
}

Connection *ConnectionEditorWidget::getConnection() const
{
    return connection;
}

void ConnectionEditorWidget::saveChanges()
{
}

void ConnectionEditorWidget::setConnection( Connection &conn )
{
    connection = &conn;

    

    setEditable( true );
}

void ConnectionEditorWidget::unsetConnection()
{
    connection = 0;
    setEditable( false );
}

void ConnectionEditorWidget::setEditable( bool set_editable )
{
    if( set_editable == is_editable )
        return;

    is_editable = set_editable;
    ui->detailsGroupBox->setEnabled( is_editable );
}

// AddConnectionDialog

AddConnectionDialog::AddConnectionDialog( QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::AddConnectionDialog )
{
    setupUi();
    setupActions();
}

void AddConnectionDialog::slotLocationChanged( const QString &text )
{
    Q_UNUSED( text )
    
    // Try should be disabled
    if( ui->hostnameLineEdit->text() == "" || ui->portLineEdit->text() == "" )
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
    emit( addConnection( ui->labelLineEdit->text(), ui->hostnameLineEdit->text(), ui->portLineEdit->text().toUInt() ) );
}

void AddConnectionDialog::setupUi()
{
    QWidget *widget = new QWidget( this );
    ui->setupUi( widget );
    setMainWidget( widget );
    
    setButtons( KDialog::Ok | KDialog::Close );
    
    button( KDialog::Ok )->setEnabled( false );
}

void AddConnectionDialog::setupActions()
{
    connect( this, SIGNAL( okClicked() ), this, SLOT( tryConnecting() ) );
    connect( ui->hostnameLineEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotLocationChanged( const QString& ) ) );
    connect( ui->portLineEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( slotLocationChanged( const QString& ) ) );
}

// ConnectionManagerWidget

ConnectionManagerWidget::ConnectionManagerWidget( InfinoteManager &manager,
    QWidget *parent )
    : QWidget( parent )
    , infinoteManager( &manager )
    , connectionListWidget( new ConnectionListWidget( manager, this ) )
    , addConnectionDialog( 0 )
{
    setupUi();
    setupActions();
}

ConnectionListWidget &ConnectionManagerWidget::getListWidget() const
{
    return *connectionListWidget;
}

void ConnectionManagerWidget::slotAddConnection()
{
    if( addConnectionDialog )
    {
        kDebug() << "Add connection dialog already opened.";
        return;
    }
    
    addConnectionDialog = new AddConnectionDialog();
    connect( addConnectionDialog, SIGNAL( finished() ), this, SLOT( slotAddConnectionFinished() ) );
    connect( addConnectionDialog, SIGNAL( addConnection( const QString&, const QString&, unsigned int ) ),
        infinoteManager, SLOT( connectToHost( const QString&, const QString&, unsigned int ) ) );
    addConnectionDialog->setVisible( true );
}

void ConnectionManagerWidget::slotAddConnectionFinished()
{
    addConnectionDialog = 0;
}

void ConnectionManagerWidget::slotRemoveConnection()
{
    QList<QListWidgetItem*> selected = connectionListWidget->selectedItems();
    QList<QListWidgetItem*>::iterator itr;

    for( itr = selected.begin(); itr != selected.end(); ++itr )
    {
        ConnectionListWidgetItem* item = static_cast<ConnectionListWidgetItem*>(*itr);
        infinoteManager->removeConnection( item->getConnection() );
    }
}

void ConnectionManagerWidget::slotControlConnection()
{
    ConnectionListWidgetItem* item = dynamic_cast<ConnectionListWidgetItem*>(connectionListWidget->currentItem());
    if( item->getConnection().isConnected() )
        item->getConnection().close();
    else
        item->getConnection().open();

    slotItemSelectionChanged();
}

void ConnectionManagerWidget::slotItemSelectionChanged()
{
    QList<QListWidgetItem*> items = connectionListWidget->selectedItems();

    if( items.size() )
    {
        removeButton->setEnabled( true );
        if( items.size() == 1 )
        {
            controlButton->setEnabled( true );
            if( dynamic_cast<ConnectionListWidgetItem*>(items.at(0))->getConnection().isConnected() )
                controlButton->setText( "Disconnect" );
            else
                controlButton->setText( "Reconnect" );
        }
    }
    else
    {
        removeButton->setEnabled( false );
        controlButton->setEnabled( false );
    }
}

void ConnectionManagerWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    addButton = new QPushButton( KIcon( "list-add.png" ), "Add", this );
    removeButton = new QPushButton( KIcon( "list-remove.png" ), "Remove", this );
    removeButton->setEnabled( false );
    controlButton = new QPushButton( KIcon( "network-disconnect.png" ), "Disconnect", this );
    controlButton->setEnabled( false );

    buttonLayout->addWidget( addButton );
    buttonLayout->addWidget( removeButton );
    buttonLayout->addWidget( controlButton );
    buttonLayout->addStretch();

    mainLayout->addWidget( connectionListWidget );
    mainLayout->addLayout( buttonLayout );

    setLayout( mainLayout );

    adjustSize();
}

void ConnectionManagerWidget::setupActions()
{
    connect( addButton, SIGNAL( clicked() ), this, SLOT( slotAddConnection() ) );
    connect( removeButton, SIGNAL( clicked() ), this, SLOT( slotRemoveConnection() ) );
    connect( controlButton, SIGNAL( clicked() ), this, SLOT( slotControlConnection() ) );
    connect( connectionListWidget, SIGNAL( itemSelectionChanged() ),
        this, SLOT( slotItemSelectionChanged() ) );
}

ConnectionManagerDialog::ConnectionManagerDialog( InfinoteManager &manager, QWidget *parent )
    : KDialog( parent )
    , infinoteManager( &manager )
{
    setupUi();
}

void ConnectionManagerDialog::setupUi()
{
    connectionManagerWidget = new ConnectionManagerWidget( *infinoteManager, this );

    setCaption( "Connection Manager" );
    setMainWidget( connectionManagerWidget );
    setButtons( KDialog::Ok );

    adjustSize();
}

} // namespace Kobby
