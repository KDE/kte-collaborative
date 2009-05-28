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

#ifndef KOBBY_DOCUMENTTABWIDGET_H
#define KOBBY_DOCUMENTTABWIDGET_H

#include <KTabWidget>
#include <QHash>

namespace KTextEditor
{
    class Document;
    class View;
}

namespace Kobby
{

class Document;

/**
 * @brief Tab widget containing active document views, and map from documents to their view.
 */
class DocumentTabWidget
    : public KTabWidget
{
    Q_OBJECT;

    public:
        DocumentTabWidget( QWidget *parent = 0 );
        ~DocumentTabWidget();

        KTextEditor::View *viewAt( int index );
        KTextEditor::View *activeView();

    Q_SIGNALS:
        void viewRemoved( KTextEditor::View &view );
        void viewAdded( KTextEditor::View &view );
        /**
         * @brief Active view has been changed.
         *
         * Emits 0 if no view is active.
         */
        void viewActivated( KTextEditor::View *view );

    public Q_SLOTS:
        /**
         * @brief Create new view of document and add as a new tab.
         */
        void addDocument( Document &doc );

        /**
         * @brief Remove all views of document
         */
        void removeDocument( Document &doc );

    private Q_SLOTS:
        void closeWidget( QWidget *widget );
        void addDocument( KTextEditor::Document &document, QString name );
        void removeDocument( KTextEditor::Document &document );
        void slotCurrentTabChanged( int index );

};

}

#endif
