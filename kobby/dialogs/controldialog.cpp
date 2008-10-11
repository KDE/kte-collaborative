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

ControlWidget::ControlWidget( QWidget *parent )
    : QWidget( parent )
    , ui( new Ui::ControlWidget )
{
    infinoteManager = InfinoteManager::instance( this );
    ui->setupUi( this );
    setupUi();
    setupActions();
}

void ControlWidget::setupUi()
{
    connectionManagerWidget = new ConnectionManagerWidget( this );

    QVBoxLayout *connectionsGroupLayout = new QVBoxLayout( ui->connectionsGroupBox );
    
    connectionsGroupLayout->addWidget(connectionManagerWidget);
}

void ControlWidget::setupActions()
{
    connect( &connectionManagerWidget->getListWidget(), SIGNAL( connectionSelectionChanged( Connection* ) ),
        &ui->browseTab->getTreeWidget(), SLOT( setConnection( Connection* ) ) );
}

ControlDialog::ControlDialog( QWidget *parent )
    : KDialog( parent )
{
    infinoteManager = InfinoteManager::instance( this );
    setupUi();
}

void ControlDialog::setupUi()
{
    setCaption( "Kobby Control" );
    controlWidget = new ControlWidget( this );
    setMainWidget( controlWidget );
    setButtons( KDialog::Ok );
}

}
