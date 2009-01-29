#include "settingsdialog.h"
#include "kobbysettings.h"

#include <KLocalizedString>
#include <KIcon>

#include "ui_profilesettingswidget.h"

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

SettingsDialog::SettingsDialog( QWidget *parent )
    : KConfigDialog( parent, "Kobby Settings", KobbySettings::self() )
{
    setupUi();
}

void SettingsDialog::slotApplyClicked()
{
    saveSettings();
}

void SettingsDialog::setupUi()
{
    profilePage = addPage( new ProfileSettings( this ), i18n("Profile") );
    profilePage->setIcon( KIcon( "user-identity.png" ) );
}

void SettingsDialog::setupSignals()
{
    connect( this, SIGNAL(applyClicked()),
        this, SLOT(slotApplyClicked()) );
}

void SettingsDialog::saveSettings()
{
}

}

