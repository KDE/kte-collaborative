#include <libinfinitymm/common/tcpconnection.h>
#include <libinfinitymm/common/xmppconnection.h>

#include "connectionmanager.h"

#include "kobby/infinote/infinotemanager.h"
#include "kobby/infinote/connection.h"
#include "kobby/dialogs/addconnectiondialog.h"

#include <KDebug>
#include <KIcon>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace Kobby
{

ConnectionListWidgetItem::ConnectionListWidgetItem( const Connection &conn, QListWidget *parent )
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

const Connection &ConnectionListWidgetItem::getConnection()
{
    return *connection;
}

void ConnectionListWidgetItem::slotStatusChanged( int status )
{
    Q_UNUSED( status )
    setDisplay();
}

ConnectionListWidget::ConnectionListWidget( InfinoteManager &manager, QWidget *parent )
    : QListWidget( parent )
    , infinoteManager( &manager )
{
    addConnections( infinoteManager->getConnections() );
    setupActions();
}

void ConnectionListWidget::addConnection( const Connection &connection )
{
    addItem( new ConnectionListWidgetItem( connection ) );
}

void ConnectionListWidget::removeConnection( const Connection &connection )
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

void ConnectionListWidget::addConnections( const QList<Connection*> &connections )
{
    QList<Connection*>::const_iterator itr;

    for( itr = connections.begin(); itr != connections.end(); ++itr )
        addItem( new ConnectionListWidgetItem( **itr ) );
}

void ConnectionListWidget::setupActions()
{
    connect( infinoteManager, SIGNAL( connectionAdded( const Connection & ) ),
        this, SLOT( addConnection( const Connection & ) ) );
    connect( infinoteManager, SIGNAL( connectionRemoved( const Connection & ) ),
        this, SLOT( removeConnection( const Connection & ) ) );
}

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

void ConnectionManagerWidget::slotAddConnection()
{
    if( addConnectionDialog )
    {
        kDebug() << "Add connection dialog already opened.";
        return;
    }
    
    addConnectionDialog = new AddConnectionDialog();
    connect( addConnectionDialog, SIGNAL( finished() ), this, SLOT( slotAddConnectionFinished() ) );
    connect( addConnectionDialog, SIGNAL( addConnection( const QString &, const QString &, unsigned int ) ),
        infinoteManager, SLOT( connectToHost( const QString &, const QString &, unsigned int ) ) );
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

void ConnectionManagerWidget::slotItemSelectionChanged()
{
    QList<QListWidgetItem*> items = connectionListWidget->selectedItems();

    if( items.size() )
        removeButton->setEnabled( true );
    else
        removeButton->setEnabled( false );
}

void ConnectionManagerWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    addButton = new QPushButton( KIcon( "list-add.png" ), "Add", this );
    removeButton = new QPushButton( KIcon( "list-remove.png" ), "Remove", this );
    removeButton->setEnabled( false );

    buttonLayout->addWidget( addButton );
    buttonLayout->addWidget( removeButton );
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

    setMainWidget( connectionManagerWidget );
    setButtons( KDialog::Ok );

    adjustSize();
}

} // namespace Kobby
