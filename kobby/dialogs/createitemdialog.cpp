#include "createitemdialog.h"

#include <QWidget>

#include "ui_createitemwidget.h"

namespace Kobby
{

CreateItemDialog::CreateItemDialog( QString title,
    QString label,
    QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CreateItemWidget )
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    ui->itemNameLabel->setText( label );
    setMainWidget( mainWidget );
    setWindowTitle( title );
}

QString CreateItemDialog::name() const
{
    return ui->nameLineEdit->text();
}

}

