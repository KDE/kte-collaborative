#include <libqinfinitymm/browsermodel.h>
#include "filebrowserwidget.h"

#include <libqinfinitymm/browseritem.h>

#include <KDebug>

#include <QVBoxLayout>
#include <QTreeView>

#include "filebrowserwidget.moc"

namespace Kobby
{

FileBrowserWidget::FileBrowserWidget( QWidget *parent )
    : QWidget( parent )
    , fileModel( new QInfinity::BrowserModel( this ) )
{
    setupUi();
    setupActions();
}

void FileBrowserWidget::setupUi()
{
    QVBoxLayout *vertLayout = new QVBoxLayout( this );
    m_treeView = new QTreeView( this );
    m_treeView->setModel( fileModel );

    vertLayout->addWidget( m_treeView );
    setLayout( vertLayout );
}

void FileBrowserWidget::setupActions()
{
    connect( m_treeView, SIGNAL(doubleClicked( const QModelIndex& )),
        fileModel, SLOT(openItem( const QModelIndex& )) );
    connect( m_treeView, SIGNAL(expanded( const QModelIndex& )),
        fileModel, SLOT(openItem( const QModelIndex& )) );
}

}

