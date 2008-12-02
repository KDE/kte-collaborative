#include "createitemdialog.h"

#include <libqinfinitymm/browseritem.h>

#include <QWidget>

#include "ui_createitemwidget.h"

namespace Kobby
{

CreateItemDialog::CreateItemDialog( QInfinity::BrowserFolderItem &parentItem,
    QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CreateItemWidget )
    , m_parentItem( &parentItem )
{
    setupUi();
    setupActions();
}

CreateItemDialog::CreateItemDialog( QInfinity::BrowserFolderItem &parentItem,
    const QString &label,
    QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CreateItemWidget )
    , m_parentItem( &parentItem )
{
    setupUi();
    setupActions();
    ui->itemNameLabel->setText( label );
}

void CreateItemDialog::slotOkClicked()
{
    emit(create( *m_parentItem, ui->itemNameLineEdit->text() ));
}

CreateItemDialog::CreateItemDialog( QInfinity::BrowserFolderItem &parentItem,
    const QString &title,
    const QString &label,
    QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CreateItemWidget )
    , m_parentItem( &parentItem )
{
    setupUi();
    setupActions();
    ui->itemNameLabel->setText( label );
}

void CreateItemDialog::setupUi()
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    setMainWidget( mainWidget );
}

void CreateItemDialog::setupActions()
{
    connect( this, SIGNAL(okClicked()),
        this, SLOT(okClicked()) );
}

}

