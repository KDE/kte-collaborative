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

#include "settingsdialog.h"
#include "kobbysettings.h"

#include <KLocalizedString>
#include <KIcon>

#include "ui_profilesettingswidget.h"
#include "ui_networksettingswidget.h"

namespace Kobby
{

class ProfileSettings
    : public QWidget
    , public Ui::ProfileSettingsWidget
{

    public:
        ProfileSettings(QWidget *parent = 0)
            : QWidget( parent )
        {
            setupUi( this );
        }

};

class NetworkSettings
    : public QWidget
    , public Ui::NetworkSettingsWidget
{

    public:
        NetworkSettings(QWidget *parent = 0)
            : QWidget( parent )
        {
            setupUi( this );
        }

};

SettingsDialog::SettingsDialog( QWidget *parent )
    : KConfigDialog( parent, "Kobby Settings", KobbySettings::self() )
{
    setupUi();
}

void SettingsDialog::setupUi()
{
    profilePage = addPage( new ProfileSettings( this ), i18n("Profile") );
    profilePage->setIcon( KIcon( "user-identity.png" ) );
    networkPage = addPage( new NetworkSettings( this ), i18n("Network") );
    networkPage->setIcon( KIcon( "network-workgroup.png" ) );
}

}

