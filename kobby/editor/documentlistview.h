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

#ifndef KOBBY_DOCUMENT_LIST_VIEW_H
#define KOBBY_DOCUMENT_LIST_VIEW_H

#include <QWidget>

class KAction;
class KMenu;
class QListView;
class QContextMenuEvent;
class QModelIndex;

namespace Kobby
{

class DocumentModel;
class Document;

class DocumentListView
    : public QWidget
{
    Q_OBJECT;

    public:
        DocumentListView( DocumentModel &model,
            QWidget *parent = 0 );

    Q_SIGNALS:
        void documentDoubleClicked( Document &doc );

    protected:
        void contextMenuEvent( QContextMenuEvent *e );

    private Q_SLOTS:
        void closeSelected();
        void slotDoubleClicked( const QModelIndex &index );

    private:
        void setupUi();
        void setupActions();

        KAction *closeAction;

        QListView *listView;
        DocumentModel *docModel;
        KMenu *contextMenu;

};

}

#endif

