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

#include "createconnectiondialog.h"

#include <KIcon>
#include <KLocalizedString>

#include "ui_createconnectionwidget.h"
#include "createconnectiondialog.moc"

namespace Kobby
{

CreateConnectionDialog::CreateConnectionDialog( QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CreateConnectionWidget )
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    setMainWidget( mainWidget );
    setWindowIcon( KIcon("network-connect.png") );
    setCaption( i18n("Create Connection") );
    ui->hostnameLineEdit->setFocus(Qt::MouseFocusReason);
    setupActions();
    sizeHint();
}

void CreateConnectionDialog::slotOkClicked()
{
    if( verifyInput() )
        emit(createConnection( ui->hostnameLineEdit->text(),
            ui->portLineEdit->text().toUInt() ));
}

void CreateConnectionDialog::setupActions()
{
    connect( this, SIGNAL(okClicked()), this, SLOT(slotOkClicked()) );
}

bool CreateConnectionDialog::verifyInput()
{
    return true;
}

}
