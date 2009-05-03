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
#include <QStandardItem>

namespace Kobby
{

class Document;

class DocumentItem
    : public QObject
    , public QStandardItem
{
    public:
        enum Type
        {
            KobbyDocument = 1001
        };

        DocumentItem( Document &doc );
        ~DocumentItem();

        int type() const;
        Document &document() const;

    private:
        Document *m_document;

};

class DocumentModel
    : public QStandardItemModel
{
    Q_OBJECT;

    public:
        DocumentModel( QObject *parent = 0 );

    Q_SIGNALS:
        void documentAdded( Document &document );
        void documentAboutToBeRemoved( Document &document );

    public Q_SLOTS:
        void insertDocument( Document &document );

};

}

#endif

