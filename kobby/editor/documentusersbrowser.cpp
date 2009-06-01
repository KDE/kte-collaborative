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
#include "documentmodel.h"

#include <KLocalizedString>

#include <QStackedLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "documentusersbrowser.moc"

namespace Kobby
{

DocumentUsersBrowser::DocumentUsersBrowser( DocumentModel &docModel,
    QWidget *parent )
    : QWidget( parent )
    , m_docModel( &docModel )
{
    QWidget *noActiveWidget = new QWidget( this );
    QVBoxLayout *noActiveLayout = new QVBoxLayout( noActiveWidget );
    QLabel *label = new QLabel( 
        i18n( "You must be editing a collaborative document to see active users." ) );
    label->setWordWrap( true );
    noActiveLayout->addWidget( label );
    noActiveWidget->setLayout( noActiveLayout );
    QStackedLayout *mainLayout = new QStackedLayout( this );
    mainLayout->addWidget( noActiveWidget );
    setLayout( mainLayout );
}

void DocumentUsersBrowser::setActiveDocument( Document &document )
{
}

}
