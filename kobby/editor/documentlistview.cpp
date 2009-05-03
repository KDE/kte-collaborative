/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "documentlistview.h"
#include "documentmodel.h"

#include <KAction>
#include <KIcon>
#include <KLocalizedString>
#include <KMenu>

#include <QListView>
#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QItemSelectionModel>
#include <QModelIndex>

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

void DocumentListView::closeSelected()
{
    QList<QModelIndex> selected = listView->selectionModel()->selectedIndexes();
    QModelIndex index;
    foreach( index, selected );
    {
        docModel->removeRow( index.row() );
    }
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
    closeAction->setEnabled( true );
    connect( closeAction, SIGNAL(triggered(bool)),
        this, SLOT(closeSelected()) );
}

}

