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

#include "documentusersbrowser.h"
#include "document.h"

#include <libqinfinity/usersmodel.h>
#include <libqinfinity/textsession.h>

#include <KLocalizedString>

#include <QStackedLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QLabel>

#include "documentusersbrowser.moc"

namespace Kobby
{

DocumentUsersBrowser::DocumentUsersBrowser( QWidget *parent )
    : QWidget( parent )
{
    // Create no active widget
    noActiveWidget = new QWidget( this );
    QVBoxLayout *noActiveLayout = new QVBoxLayout( noActiveWidget );
    QLabel *label = new QLabel( 
        i18n( "You must be editing a collaborative document to see active users." ) );
    label->setWordWrap( true );
    noActiveLayout->addWidget( label );
    noActiveWidget->setLayout( noActiveLayout );

    // Create browser widget
    browserWidget = new QWidget( this );
    browserList = new QListView();
    QVBoxLayout *browserLayout = new QVBoxLayout( browserWidget );
    browserLayout->addWidget( browserList );

    mainLayout = new QStackedLayout( this );
    mainLayout->addWidget( noActiveWidget );
    mainLayout->addWidget( browserWidget );
    mainLayout->setCurrentWidget( noActiveWidget );
    setLayout( mainLayout );
}

DocumentUsersBrowser::~DocumentUsersBrowser()
{
}

void DocumentUsersBrowser::setActiveDocument( Document &document )
{
    QInfinity::UsersModel *newModel;
    if( document.type() == Document::KDocument )
    {
        mainLayout->setCurrentWidget( noActiveWidget );
    }
    else
    {
        if( !documentToModel.contains( &document ) )
        {
            newModel = new QInfinity::UsersModel( *(dynamic_cast<InfTextDocument*>(&document)->infSession()), this );
            documentToModel[&document] = newModel;
        }
        else
            newModel = documentToModel[&document];
        
        browserList->setModel( newModel );
        mainLayout->setCurrentWidget( browserWidget );
    }
}

}
