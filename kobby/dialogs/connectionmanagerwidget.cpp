#include "connectionmanagerwidget.h"
#include "createconnectiondialog.h"

#include <libqinfinitymm/infinotemanager.h>
#include <libqinfinitymm/connectionmanager.h>

#include <KIcon>
#include <KGuiItem>
#include <KPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "connectionmanagerwidget.moc"

namespace Kobby
{

ConnectionManagerWidget::ConnectionManagerWidget( QWidget *parent )
    : QWidget( parent )
{
    setupUi();
    setupActions();
}

void ConnectionManagerWidget::slotSelectionChanged()
{
    removeConnectionButton->setEnabled( connectionListWidget->selectedItems().size() != 0 );
}

void ConnectionManagerWidget::slotCreateConnection()
{
    CreateConnectionDialog *dialog = new CreateConnectionDialog( this );
    dialog->setVisible( true );
}

void ConnectionManagerWidget::slotRemoveConnection()
{
    QList<QListWidgetItem*> items = connectionListWidget->selectedItems();
    QList<QListWidgetItem*>::Iterator itr;
    QInfinity::ConnectionListWidgetItem *connectionItem;

    for( itr = items.begin(); itr != items.end(); ++itr )
    {
        connectionItem = dynamic_cast<QInfinity::ConnectionListWidgetItem*>(*itr);
        QInfinity::InfinoteManager::instance()->removeConnection( connectionItem->connection() );
    }
}

void ConnectionManagerWidget::setupUi()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QVBoxLayout *vertLayout = new QVBoxLayout;
    connectionListWidget = new QInfinity::ConnectionListWidget( this );
    connectionListWidget->setVisible( true );
    
    addConnectionButton = new KPushButton( KIcon( "list-add.png" ), "",  this );
    removeConnectionButton = new KPushButton( KIcon( "list-remove.png" ), "", this );
    removeConnectionButton->setEnabled( false );
    buttonLayout->addWidget( addConnectionButton );
    buttonLayout->addWidget( removeConnectionButton );
    buttonLayout->addStretch();
    
    vertLayout->addWidget( connectionListWidget );
    vertLayout->addItem( buttonLayout );

    setLayout( vertLayout );
}

void ConnectionManagerWidget::setupActions()
{
    connect( addConnectionButton, SIGNAL(clicked()),
        this, SLOT(slotCreateConnection()) );
    connect( removeConnectionButton, SIGNAL(clicked()),
        this, SLOT(slotRemoveConnection()) );
    connect( connectionListWidget, SIGNAL(itemSelectionChanged()),
        this, SLOT(slotSelectionChanged()) );
}

}
