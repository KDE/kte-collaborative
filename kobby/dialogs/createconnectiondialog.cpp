#include "createconnectiondialog.h"

#include <libqinfinitymm/infinotemanager.h>

#include "ui_createconnectionwidget.h"
#include "createconnectiondialog.moc"

namespace Kobby
{

CreateConnectionDialog::CreateConnectionDialog( QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CreateConnectionWidget )
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    setMainWidget( mainWidget );
    setupActions();
    sizeHint();
}

void CreateConnectionDialog::slotOkClicked()
{
    QInfinity::InfinoteManager *infinoteManager = QInfinity::InfinoteManager::instance();
    
    infinoteManager->connectToHost( ui->nameLineEdit->text(), ui->hostnameLineEdit->text(), ui->portLineEdit->text().toUInt() );
}

void CreateConnectionDialog::setupActions()
{
    connect( this, SIGNAL(okClicked()), this, SLOT(slotOkClicked()) );
}

}
