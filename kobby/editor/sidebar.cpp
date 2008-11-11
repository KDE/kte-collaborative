#include "sidebar.h"

#include <libqinfinitymm/filebrowsermodel.h>
#include <libqinfinitymm/connectionmanager.h>

#include <QTreeView>

namespace Kobby
{

Sidebar::Sidebar( QWidget *parent )
    : QTabWidget( parent )
    , m_treeView( new QTreeView( this ) )
    , m_connectionList( new QInfinity::ConnectionListWidget( this ) )
{
    m_treeView->setModel( QInfinity::FileBrowserModel::instance() );
    setTabPosition( QTabWidget::South );
    addTab( m_connectionList, "Connections" );
    addTab( m_treeView, "Browse" );
}

}

