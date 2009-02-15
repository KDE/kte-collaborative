#include "remotebrowserview.h"

#include <libqinfinity/browsermodel.h>

#include <KAction>
#include <KIcon>
#include <KLocalizedString>
#include <KToolBar>

#include <QTreeView>
#include <QVBoxLayout>

#include "remotebrowserview.moc"

namespace Kobby
{

RemoteBrowserView::RemoteBrowserView( QInfinity::BrowserModel &model,
    QWidget *parent )
    : QWidget( parent )
    , m_treeView( new QTreeView( this ) )
    , browserModel( &model )
{
    m_treeView->setModel( &model );
    connect( m_treeView, SIGNAL(expanded(const QModelIndex&)),
        browserModel, SLOT(itemActivated(const QModelIndex&)) );
    setupActions();
    setupToolbar();

    QVBoxLayout *vertLayout = new QVBoxLayout( this );
    vertLayout->addWidget( toolBar );
    vertLayout->addWidget( m_treeView );

    setLayout( vertLayout );
}

void RemoteBrowserView::slotNewConnection()
{
    emit(createConnection());
}

void RemoteBrowserView::slotNewDocument()
{
}

void RemoteBrowserView::slotNewFolder()
{
}

void RemoteBrowserView::slotOpen()
{
}

void RemoteBrowserView::slotDelete()
{
}

void RemoteBrowserView::setupActions()
{
    createConnectionAction = new KAction( i18n("New Connection"), this );
    createDocumentAction = new KAction( i18n("New Document"), this );
    createFolderAction = new KAction( i18n("New Folder"), this );
    openAction = new KAction( i18n("Open Document"), this );
    deleteAction = new KAction( i18n("Delete"), this );

    createConnectionAction->setIcon( KIcon("network-connect.png") );
    createDocumentAction->setIcon( KIcon("document-new.png") );
    createFolderAction->setIcon( KIcon("folder-new.png") );
    openAction->setIcon( KIcon("document-open.png") );
    deleteAction->setIcon( KIcon("user-trash.png") );

    connect( createConnectionAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewConnection()) );
    connect( createDocumentAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewDocument()) );
    connect( createFolderAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewFolder()) );
    connect( openAction, SIGNAL(triggered(bool)),
        this, SLOT(slotOpen()) );
    connect( deleteAction, SIGNAL(triggered(bool)),
        this, SLOT(slotDelete()) );
}

void RemoteBrowserView::setupToolbar()
{
    toolBar = new KToolBar( this );
    toolBar->setIconDimensions( 16 );
    toolBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
    toolBar->addAction( createConnectionAction );
    toolBar->addAction( createDocumentAction );
    toolBar->addAction( createFolderAction );
    toolBar->addAction( openAction );
    toolBar->addAction( deleteAction );
}

}

