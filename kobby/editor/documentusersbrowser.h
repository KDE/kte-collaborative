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

#ifndef KOBBY_DOCUMENT_USERS_BROWSER_H
#define KOBBY_DOCUMENT_USERS_BROWSER_H

#include <QWidget>
#include <QHash>

#include <libqinfinity/usersmodel.h>

class QTableView;
class QStackedLayout;
class QSortFilterProxyModel;

namespace QInfinity
{
    class User;
}

namespace Kobby
{

class Document;
class UserItemFactory;

class UserItem
    : public QObject
    , public QInfinity::UserItem
{
    Q_OBJECT

    public:
        UserItem( QInfinity::User &user );

    private Q_SLOTS:
        void statusChanged();

};

class DocumentUsersBrowser
    : public QWidget
{
    Q_OBJECT

    public:
        DocumentUsersBrowser( QWidget *parent = 0 );
        ~DocumentUsersBrowser();

    public Q_SLOTS:
        /**
         * @brief Set document as current.
         *
         * If document is 0 no active document is assumed.
         */
        void setActiveDocument( Document *document );
        void removeDocument( Document &document );

    private:
        QWidget *noActiveWidget;
        QWidget *browserWidget;
        QStackedLayout *mainLayout;
        QTableView *browserList;
        QHash<Document*, QSortFilterProxyModel*> documentToModel;
        UserItemFactory *itemFactory;

};

}

#endif
