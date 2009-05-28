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

#include "documenttabwidget.h"
#include "document.h"

#include <KIcon>
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KDebug>

#include <QToolButton>

#include "documenttabwidget.moc"

namespace Kobby
{

DocumentTabWidget::DocumentTabWidget( QWidget *parent )
    : KTabWidget( parent )
{
    connect( this, SIGNAL(closeRequest( QWidget* )),
        this, SLOT(closeWidget( QWidget* )) );
    connect( this, SIGNAL(currentChanged( int )),
        this, SLOT(slotCurrentTabChanged( int )) );
    setCloseButtonEnabled( true );
}

DocumentTabWidget::~DocumentTabWidget()
{
}

KTextEditor::View *DocumentTabWidget::viewAt( int index )
{
    if( index == -1 )
        return 0;
    return dynamic_cast<KTextEditor::View*>(widget( index ));
}

KTextEditor::View *DocumentTabWidget::activeView()
{
    return dynamic_cast<KTextEditor::View*>(currentWidget());
}

void DocumentTabWidget::addDocument( Document &doc )
{
    addDocument( *doc.kDocument(), doc.name()  );
}

void DocumentTabWidget::removeDocument( Document &doc )
{
}

void DocumentTabWidget::closeWidget( QWidget *cw )
{
    int tab = indexOf( cw );
    emit( viewRemoved( *dynamic_cast<KTextEditor::View*>(cw) ) );
    removeTab( tab );
}

void DocumentTabWidget::addDocument( KTextEditor::Document &document, QString name )
{
    KTextEditor::View *view = document.createView( this );
    int tab;
    tab = addTab( view, name );
    setCurrentIndex( tab );
    emit( viewAdded( *view ) );
}

void DocumentTabWidget::removeDocument( KTextEditor::Document &document )
{
}

void DocumentTabWidget::slotCurrentTabChanged( int index )
{
    KTextEditor::View *activeView = viewAt( index );
    emit( viewActivated( activeView ) );
}

}
