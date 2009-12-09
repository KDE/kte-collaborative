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

#include "setupdialog.h"
#include "kobbysettings.h"

#include <KDebug>

#include <QWidget>
#include <QLineEdit>

#include "ui_setuppagewidget.h"
#include "ui_profilesetupwidget.h"

#include "setupdialog.moc"

namespace Kobby
{

SetupDialog::SetupDialog( QWidget *parent )
    : KAssistantDialog( parent )
    , profileUi( new Ui::ProfileSetupWidget )
    , setupUi( new Ui::SetupPageWidget )
{
    profilePage = new QWidget();
    QWidget *setupPage = new QWidget();
    profileUi->setupUi( profilePage );
    setupUi->setupUi( setupPage );
    profileUi->nicknameEdit->setText( KobbySettings::nickName() );
    profileUi->hostnameEdit->setText( KobbySettings::hostName() );
    connect( profileUi->nicknameEdit, SIGNAL(textChanged( const QString& )),
        this, SLOT(slotProfileTextEntered( const QString& )) );
    connect( this, SIGNAL(finished()),
        this, SLOT(slotFinished()) );
    addPage( setupPage, "Welcome!" );
    profilePageItem = addPage( profilePage, "Profile Settings" );
    slotProfileTextEntered( QString() );
}

void SetupDialog::slotFinished()
{
    KobbySettings::setNickName( profileUi->nicknameEdit->text() );
    KobbySettings::setHostName( profileUi->hostnameEdit->text() );
    KobbySettings::setUserColor( profileUi->colorCombo->color() );
    KobbySettings::self()->writeConfig();
}

void SetupDialog::slotProfileTextEntered( const QString &text )
{
    Q_UNUSED(text)
    if( !profileUi->nicknameEdit->text().isEmpty() )
        setValid( profilePageItem, true );
    else
        setValid( profilePageItem, false );
}

}
