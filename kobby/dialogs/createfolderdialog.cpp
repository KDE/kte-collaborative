#include "createfolderdialog.h"

#include <QWidget>

#include "ui_createfolderwidget.h"

namespace Kobby
{

CreateFolderDialog::CreateFolderDialog( QWidget *parent = 0 )
    : KDialog( parent )
    , ui( new Ui::CreateFolderWidget )
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    setMainWidget( mainWidget );
}

const QString &CreateFolderDialog::folderName() const
{
    return ui->nameLineEdit;
}

}

