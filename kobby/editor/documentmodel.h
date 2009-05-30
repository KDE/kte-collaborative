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

#ifndef KOBBY_DOCUMENT_MODEL_H
#define KOBBY_DOCUMENT_MODEL_H

#include <QStandardItemModel>
#include <QHash>

namespace KTextEditor
{
    class Document;
}

namespace Kobby
{

class Document;
class DocumentItem;

class DocumentModel
    : public QStandardItemModel
{
    Q_OBJECT;

    public:
        DocumentModel( QObject *parent = 0 );

        void removeKDocument( KTextEditor::Document &kDoc );
        Document *documentFromIndex( const QModelIndex &index );

    Q_SIGNALS:
        void documentAdded( Document &document );
        void documentAboutToBeRemoved( Document &document );

    public Q_SLOTS:
        void insertDocument( Document &document );

    private Q_SLOTS:
        void slotRowsAboutToBeRemoved( const QModelIndex& parent, int start, int end );

    private:
        QHash<KTextEditor::Document*, DocumentItem*> m_kDocumentItemWrappers;
};

}

#endif

