/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
