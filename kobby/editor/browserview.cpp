#include "browserview.h"

#include <QTreeView>
#include <QVBoxLayout>

namespace Kobby
{

BrowserView::BrowserView( QInfinity::FileModel &model,
    QWidget *parent )
    : QWidget( parent )
    , m_treeView( new QTreeView( this ) )
{
    m_treeView->setModel( &model );

    QVBoxLayout *vertLayout = new QVBoxLayout( this );
    vertLayout->addWidget( m_treeView );

    setLayout( vertLayout );
}

}

