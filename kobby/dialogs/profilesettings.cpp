#include "profilesettings.h"

#include "ui_profilesettingswidget.h"

namespace Kobby
{

ProfileSettings::ProfileSettings( QWidget *parent )
    : QWidget( parent )
    , ui( new Ui::ProfileSettingsWidget )
{
    ui->setupUi( this );
}

}

