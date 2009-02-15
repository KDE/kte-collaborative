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

