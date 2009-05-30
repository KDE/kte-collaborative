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

/**
 * @brief Handles storage and deletion of all Documents.
 */
class DocumentModel
    : public QStandardItemModel
{
    Q_OBJECT;

    public:
        DocumentModel( QObject *parent = 0 );

        /**
         * @brief Remove Document wrapping kDoc from model.
         */
        void removeKDocument( KTextEditor::Document &kDoc );

        /**
         * @brief Retrieve Document at index.
         */
        Document *documentFromIndex( const QModelIndex &index );

    Q_SIGNALS:
        /**
         * @brief A Document has been added.
         */
        void documentAdded( Document &document );

        /**
         * @brief A fatal error has occoured with a document.
         */
        void documentFatalError( Document *doc, QString reason );

        /**
         * @brief A Document is about to be removed.
         */
        void documentAboutToBeRemoved( Document &document );

    public Q_SLOTS:
        /**
         * @brief Appends document to model.
         *
         * Model takes ownership of document.
         */
        void insertDocument( Document &document );

    private Q_SLOTS:
        void slotRowsAboutToBeRemoved( const QModelIndex& parent, int start, int end );
        void slotDocumentFatalError( Document *document,
            QString message );

    private:
        QHash<KTextEditor::Document*, DocumentItem*> m_kDocumentItemWrappers;
};

}

#endif

