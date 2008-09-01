#include "settingsdialog.h"

#include "kobby/ui_settingswidget.h"
#include "kobby/ui_usersettingswidget.h"

#include <QGridLayout>

namespace Kobby
{

SettingsDialog::SettingsDialog( QWidget *parent, const QVariantList &args )
    : KCModule( PluginFactory::componentData(), parent, args )
    , settingsWidgetUi( new Ui::SettingsWidget )
    , userSettingsWidgetUi( new Ui::UserSettingsWidget )
{
    settingsWidgetUi->setupUi( this );

    // Create UI of user settings tab
    userSettingsWidgetUi->setupUi( settingsWidgetUi->settingsTabWidget->currentWidget() );
    settingsWidgetUi->settingsTabWidget->adjustSize();

    adjustSize();
}

}
