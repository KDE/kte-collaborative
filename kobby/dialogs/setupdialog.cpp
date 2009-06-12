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

#include "ui_setuppagewidget.h"
#include "ui_profilesetupwidget.h"
#include "setupdialog.moc"

namespace Kobby
{

class SetupPage
    : public QWidget
    , private Ui::SetupPageWidget
{

    public:
        SetupPage();

};

class ProfileSetupPage
    : public QWidget
    , public Ui::ProfileSetupWidget
{

    public:
        ProfileSetupPage();

};

SetupPage::SetupPage()
    : QWidget()
{
    setupUi( this );
}

ProfileSetupPage::ProfileSetupPage()
    : QWidget()
{
    setupUi( this );
    nicknameEdit->setText( KobbySettings::nickName() );
    hostnameEdit->setText( KobbySettings::hostName() );
}

SetupDialog::SetupDialog( QWidget *parent )
    : KAssistantDialog( parent )
{
    profilePage = new ProfileSetupPage;
    connect( profilePage->nicknameEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(slotProfileTextEntered(const QString&)) );
    addPage( new SetupPage, "Welcome!" );
    profilePageItem = addPage( new ProfileSetupPage, "Profile Settings" );
}

void SetupDialog::slotProfileTextEntered( const QString &text )
{
    Q_UNUSED(text)
    kDebug() << "Text entered.";
    if( !profilePage->nicknameEdit->text().isEmpty() )
    {
        setValid( profilePageItem, true );
    }
    else
        setValid( profilePageItem, false );
}

}
