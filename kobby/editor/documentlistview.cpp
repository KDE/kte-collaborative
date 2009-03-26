#include "documentlistview.h"
#include "documentmodel.h"

#include <KAction>
#include <KIcon>
#include <KLocalizedString>
#include <KMenu>

#include <QListView>
#include <QHBoxLayout>
#include <QContextMenuEvent>

namespace Kobby
{

DocumentListView::DocumentListView( DocumentModel &model,
    QWidget *parent )
    : QWidget( parent )
    , docModel( &model )
    , contextMenu( 0 )
{
    setupUi();
    setupActions();
}

void DocumentListView::contextMenuEvent( QContextMenuEvent *e )
{
    if( !contextMenu )
    {
        contextMenu = new KMenu( this );
        contextMenu->addAction( closeAction );
    }
    contextMenu->popup( e->globalPos() );
}

void DocumentListView::setupUi()
{
    QHBoxLayout *layout = new QHBoxLayout( this );
    listView = new QListView( this );
    listView->setModel( docModel );
    layout->addWidget( listView );
    setLayout( layout );
}

void DocumentListView::setupActions()
{
    closeAction = new KAction( KIcon("dialog-close.png"),
        i18n("Close"), this );
    closeAction->setEnabled( false );
}

}

