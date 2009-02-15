#include "createconnectiondialog.h"

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
    if( verifyInput() )
        emit(createConnection( ui->hostnameLineEdit->text(),
            ui->portLineEdit->text().toUInt() ));
}

void CreateConnectionDialog::setupActions()
{
    connect( this, SIGNAL(okClicked()), this, SLOT(slotOkClicked()) );
}

bool CreateConnectionDialog::verifyInput()
{
    return true;
}

}
