#include "localbrowserview.h"

#include <KConfigGroup>
#include <KUrl>
#include <KDirOperator>
#include <KToolBar>
#include <KActionCollection>
#include <KAction>

#include <QVBoxLayout>

namespace Kobby
{

LocalBrowserView::LocalBrowserView( QWidget *parent )
    : QWidget( parent )
{
    setupUi();
}

void LocalBrowserView::setupUi()
{
    QVBoxLayout *vlayout = new QVBoxLayout( this );
    KConfigGroup grp(KGlobal::config(), "KDiroperator ConfigGroup" );
    dirOperator = new KDirOperator( KUrl("~"), this );
    dirOperator->readConfig( grp );
    dirOperator->setView( KFile::Default );

    navToolBar = new KToolBar( this );
    navToolBar->setIconDimensions( 16 );
    navToolBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
    navToolBar->addAction( dirOperator->actionCollection()->action("up") );
    navToolBar->addAction( dirOperator->actionCollection()->action("back") );
    navToolBar->addAction( dirOperator->actionCollection()->action("forward") );
    navToolBar->addAction( dirOperator->actionCollection()->action("home") );
    navToolBar->addAction( dirOperator->actionCollection()->action("reload") );
    navToolBar->addAction( dirOperator->actionCollection()->action("mkdir") );
    navToolBar->addAction( dirOperator->actionCollection()->action("view menu") );

    vlayout->addWidget( navToolBar );
    vlayout->addWidget( dirOperator );

    setLayout( vlayout );
}

}

