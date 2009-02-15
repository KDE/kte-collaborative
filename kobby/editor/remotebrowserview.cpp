#include "remotebrowserview.h"

#include <KAction>
#include <KIcon>
#include <KLocalizedString>
#include <KToolBar>

#include <QTreeView>
#include <QVBoxLayout>

namespace Kobby
{

RemoteBrowserView::RemoteBrowserView( QInfinity::BrowserModel &model,
    QWidget *parent )
    : QWidget( parent )
    , m_treeView( new QTreeView( this ) )
{
    m_treeView->setModel( &model );
    setupActions();
    setupToolbar();

    QVBoxLayout *vertLayout = new QVBoxLayout( this );
    vertLayout->addWidget( toolBar );
    vertLayout->addWidget( m_treeView );

    setLayout( vertLayout );
}

void RemoteBrowserView::setupActions()
{
    createDocumentAction = new KAction( i18n("New Document"), this );
    createDocumentAction->setIcon( KIcon("document-new.png") );
    createFolderAction = new KAction( i18n("New Folder"), this );
    createFolderAction->setIcon( KIcon("folder-new.png") );
    openAction = new KAction( i18n("Open Document"), this );
    openAction->setIcon( KIcon("document-open.png") );
    deleteAction = new KAction( i18n("Delete"), this );
    deleteAction->setIcon( KIcon("user-trash.png") );
}

void RemoteBrowserView::setupToolbar()
{
    toolBar = new KToolBar( this );
    toolBar->setIconDimensions( 16 );
    toolBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
    toolBar->addAction( createDocumentAction );
    toolBar->addAction( createFolderAction );
    toolBar->addAction( openAction );
    toolBar->addAction( deleteAction );
}

}

