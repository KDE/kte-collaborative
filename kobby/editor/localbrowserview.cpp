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
    connect( newView, SIGNAL(activated(const QModelIndex&)),
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
    navToolBar->addAction( action("home") );
    navToolBar->addAction( action("mkdir") );
    navToolBar->addAction( action("view menu") );

    vlayout->addWidget( navToolBar );
    vlayout->addWidget( dirOperator );

    setLayout( vlayout );
}

}

