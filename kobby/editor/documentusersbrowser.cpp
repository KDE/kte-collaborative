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

#include <libqinfinity/user.h>
#include <libqinfinity/usersmodel.h>
#include <libqinfinity/textsession.h>

#include <KLocalizedString>
#include <KIcon>

#include <QStackedLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QLabel>
#include <QSortFilterProxyModel>

#include "documentusersbrowser.moc"

namespace Kobby
{

class UserItem
    : public QInfinity::UserItem
{

    public:
        UserItem( QInfinity::User &user );

};

class UserItemFactory
    : public QInfinity::UserItemFactory
{

    QInfinity::UserItem *createUserItem( QInfinity::User &user );

};

UserItem::UserItem( QInfinity::User &user )
    : QInfinity::UserItem( user )
{
    if( user.status() == QInfinity::User::Active )
        setIcon( KIcon("user-online.png") );
    else if( user.status() == QInfinity::User::Inactive )
        setIcon( KIcon("user-away.png") );
    else
        setIcon( KIcon("user-offline.png") );

    setEditable( false );
    setColumnCount( 1 );
}

QInfinity::UserItem *UserItemFactory::createUserItem( QInfinity::User &user )
{
    UserItem *item = new UserItem( user );
    return item;
}

DocumentUsersBrowser::DocumentUsersBrowser( QWidget *parent )
    : QWidget( parent )
{
    itemFactory = new UserItemFactory();

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
    QSortFilterProxyModel *filter;
    if( document.type() == Document::KDocument )
    {
        mainLayout->setCurrentWidget( noActiveWidget );
    }
    else
    {
        if( !documentToModel.contains( &document ) )
        {
            newModel = new QInfinity::UsersModel( *(dynamic_cast<InfTextDocument*>(&document)->infSession()), itemFactory, this );
            filter = new QSortFilterProxyModel( this );
            filter->setSourceModel( newModel );
            documentToModel[&document] = filter;
        }
        else
            filter = documentToModel[&document];
        
        browserList->setModel( filter );
        mainLayout->setCurrentWidget( browserWidget );
    }
}

void DocumentUsersBrowser::removeDocument( Document &document )
{
    
}

}
