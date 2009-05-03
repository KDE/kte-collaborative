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

#ifndef KOBBY_DOCUMENT_H
#define KOBBY_DOCUMENT_H

#include <libqinfinity/abstracttextbuffer.h>

#include <QObject>
#include <QPointer>

#include <glib/gerror.h>

namespace KTextEditor
{
    class Document;
    class Range;
    class Cursor;
}

namespace QInfinity
{
    class Session;
    class SessionProxy;
    class User;
    class TextChunk;
    class TextBuffer;
    class BrowserIter;
}

namespace Kobby
{

/**
 * @brief A base class for interacting with Documents.
 *
 * Abstracting the document interface allows us to create
 * an interface for views to perform operations on doucments
 * without knowing the type of document being represented.
 */
class Document
{

    public:
        Document( KTextEditor::Document &kDocument );
        virtual ~Document();

        KTextEditor::Document *kDocument() const;
        virtual bool save();
        virtual QString name();
    
    private:
        QPointer<KTextEditor::Document> m_kDocument;

};

/**
 * @brief Links together the InfTextBuffer and KTextEditor::Document
 */
class KDocumentTextBuffer
    : public QInfinity::AbstractTextBuffer
    , public Document
{
    Q_OBJECT;

    public:
        KDocumentTextBuffer( KTextEditor::Document &kDocument,
            const QString &encoding,
            QObject *parent = 0 );
        ~KDocumentTextBuffer();

        void setName( const QString &name );
        void onInsertText( unsigned int offset,
            const QInfinity::TextChunk &chunk,
            QInfinity::User *user );
        void onEraseText( unsigned int offset,
            unsigned int length,
            QInfinity::User *user );
        QString name();

    Q_SIGNALS:
        void error( QString message, bool close );

    public Q_SLOTS:
        void joinFailed( GError *error );

    private Q_SLOTS:
        void localTextInserted( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void localTextRemoved( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void setUser( QPointer<QInfinity::User> user );

    private:
        unsigned int cursorToOffset( const KTextEditor::Cursor &cursor );
        KTextEditor::Cursor offsetToCursor( unsigned int offset );

        bool blockLocalInsert;
        bool blockLocalRemove;
        bool blockRemoteInsert;
        bool blockRemoteRemove;
        QPointer<QInfinity::User> m_user;
        QString m_name;

};

}

#endif

