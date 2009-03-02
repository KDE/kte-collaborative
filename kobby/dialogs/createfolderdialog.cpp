#include "createfolderdialog.h"

#include <QWidget>

#include "ui_createfolderwidget.h"
#include "createfolderdialog.moc"

namespace Kobby
{

CreateFolderDialog::CreateFolderDialog( QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CreateFolderWidget )
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    setMainWidget( mainWidget );
}

QString CreateFolderDialog::folderName() const
{
    return ui->folderNameLineEdit->text();
}

}

