#include "settingsdialog.h"

#include "kobby/ui_settingswidget.h"
#include "kobby/ui_usersettingswidget.h"

#include <KSharedConfig>

#include <QGridLayout>

namespace Kobby
{

SettingsDialog::SettingsDialog( QWidget *parent )
    : KDialog( parent )
    , settingsWidgetUi( new Ui::SettingsWidget )
    , userSettingsWidgetUi( new Ui::UserSettingsWidget )
{
    QWidget *widget = new QWidget;
    // Create UI of settings dialog
    settingsWidgetUi->setupUi( widget );

    // Create UI of user settings tab
    userSettingsWidgetUi->setupUi( settingsWidgetUi->settingsTabWidget->currentWidget() );
    settingsWidgetUi->settingsTabWidget->adjustSize();

    setMainWidget( widget );
    
    loadConfig();
    
    adjustSize();
}

void SettingsDialog::loadConfig()
{
}

}
