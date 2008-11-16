#include "sidebar.h"
#include "../dialogs/connectionmanagerwidget.h"

#include <libqinfinitymm/filebrowsermodel.h>

#include <QTreeView>

namespace Kobby
{

Sidebar::Sidebar( QWidget *parent )
    : QTabWidget( parent )
    , m_treeView( new QTreeView( this ) )
    , m_connectionManager( new ConnectionManagerWidget( this ) )
{
    m_treeView->setModel( QInfinity::FileBrowserModel::instance() );
    setTabPosition( QTabWidget::South );
    addTab( m_connectionManager, "Connections" );
    addTab( m_treeView, "Browse" );
    m_connectionManager->setVisible( true );
}

}

