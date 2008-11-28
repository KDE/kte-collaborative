#include "createfolderdialog.h"

#include <libqinfinitymm/browseritem.h>

#include <QWidget>

#include "ui_createfolderwidget.h"
#include "createfolderdialog.moc"

namespace Kobby
{

CreateFolderDialog::CreateFolderDialog( const QInfinity::BrowserFolderItem &parentItem,
    QWidget *parent )
    : KDialog( parent )
    , m_parent( &parentItem )
    , ui( new Ui::CreateFolderWidget )
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    setMainWidget( mainWidget );
    connect( this, SIGNAL(okClicked()),
        this, SLOT(slotOkClicked()) );
}

QString CreateFolderDialog::folderName() const
{
    return ui->folderNameLineEdit->text();
}

void CreateFolderDialog::slotOkClicked()
{
    emit(createFolder( *m_parent, folderName() ));
}

}

