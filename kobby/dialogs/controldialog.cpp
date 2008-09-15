#include "controldialog.h"

#include <kobby/infinote/infinotemanager.h>
#include <kobby/dialogs/connectionmanager.h>
#include <kobby/dialogs/filebrowser.h>

#include <KLocale>

#include <QVBoxLayout>
#include <QSplitter>

#include "kobby/ui_controlwidget.h"

namespace Kobby
{

ControlWidget::ControlWidget( InfinoteManager &manager, QWidget *parent )
    : QWidget( parent )
    , infinoteManager( &manager )
    , ui( new Ui::ControlWidget )
{
    ui->setupUi( this );
    setupUi();
}

void ControlWidget::setupUi()
{
    connectionManagerWidget = new ConnectionManagerWidget( *infinoteManager, this );

    QVBoxLayout *connectionsGroupLayout = new QVBoxLayout( ui->connectionsGroupBox );
    
    connectionsGroupLayout->addWidget(connectionManagerWidget);
}

ControlDialog::ControlDialog( InfinoteManager &manager, QWidget *parent )
    : KDialog( parent )
    , infinoteManager( &manager )
{
    setupUi();
}

void ControlDialog::setupUi()
{
    setCaption( "Kobby Control" );
    controlWidget = new ControlWidget( *infinoteManager, this );
    setMainWidget( controlWidget );
    setButtons( KDialog::Ok );
}

}
