#include "localbrowserview.h"

#include <KConfigGroup>
#include <KUrl>
#include <KFileItem>
#include <KDirOperator>
#include <KToolBar>
#include <KActionCollection>
#include <KAction>

#include <QVBoxLayout>
#include <QAbstractItemView>
#include <QAbstractItemModel>

namespace Kobby
{

LocalBrowserView::LocalBrowserView( QWidget *parent )
    : QWidget( parent )
{
    setupUi();
}

QAction *LocalBrowserView::action( const QString &name )
{
    return dirOperator->actionCollection()->action( name );
}

void LocalBrowserView::slotFileSelected( const QModelIndex &index )
{
    Q_UNUSED(index);
    KFileItemList list = dirOperator->selectedItems();
    QList<KFileItem>::Iterator itr;
    for( itr = list.begin(); itr != list.end(); itr++ )
        emit(urlSelected( itr->url() ));
}

void LocalBrowserView::slotViewChanged( QAbstractItemView *newView )
{
    connect( newView, SIGNAL(doubleClicked(const QModelIndex&)),
        this, SLOT(slotFileSelected(const QModelIndex&)) );
}

void LocalBrowserView::setupUi()
{
    QVBoxLayout *vlayout = new QVBoxLayout( this );
    KConfigGroup grp(KGlobal::config(), "KDiroperator ConfigGroup" );
    dirOperator = new KDirOperator( KUrl("~"), this );
    connect( dirOperator, SIGNAL(viewChanged( QAbstractItemView * )),
        this, SLOT(slotViewChanged( QAbstractItemView * )) );
    dirOperator->readConfig( grp );
    dirOperator->setView( KFile::Default );

    navToolBar = new KToolBar( this );
    navToolBar->setIconDimensions( 16 );
    navToolBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
    navToolBar->addAction( action("up") );
    navToolBar->addAction( action("back") );
    navToolBar->addAction( action("forward") );
    navToolBar->addAction( action("home") );
    navToolBar->addAction( action("reload") );
    navToolBar->addAction( action("mkdir") );
    navToolBar->addAction( action("view menu") );

    vlayout->addWidget( navToolBar );
    vlayout->addWidget( dirOperator );

    setLayout( vlayout );
}

}

