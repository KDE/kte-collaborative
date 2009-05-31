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

typedef struct _GError GError;

class QString;
class QAction;

namespace KTextEditor
{
    class Document;
    class Range;
    class Cursor;
    class View;
}

namespace QInfinity
{
    class Session;
    class SessionProxy;
    class User;
    class AdoptedUser;
    class TextChunk;
    class TextBuffer;
    class TextSession;
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
 *
 * If using this class, you must monitor for fatalError, which
 * will notify of deletion of the Document.
 */
class Document
    : public QObject
{
    Q_OBJECT
    
    public:
        enum LoadState
        {
            Unloaded, // Loading has not begun
            Synchronizing,
            SynchronizationComplete,
            Joining,
            JoiningComplete,
            Complete // Loading is complete
        };
        
        Document( KTextEditor::Document &kDocument );
        virtual ~Document();

        /**
         * @brief Get KTextEditor::Document being represented.
         */
        KTextEditor::Document *kDocument() const;
        
        /**
         * @brief Save document.
         */
        virtual bool save();
        
        /**
         * @brief Name of document.
         */
        virtual QString name();
        
        /**
         * @brief State of document loading.
         * 
         * Initial state is Unloaded.
         */
        Document::LoadState loadState() const;

    Q_SIGNALS:
        void loadStateChanged( Document *document,
            Document::LoadState loadState );
        
        /**
         * @brief Document completed loading.
         *
         * This signal will be emitted directly after a
         * loadStateChanged signal.
         */
        void loadingComplete( Document *document );
        
        /**
         * @brief Fatal error has occoured with the document.
         *
         * After this signal is recieved references to the
         * document instance are likely no longer valid.
         */
        void fatalError( Document *document,
            QString message );
    
    protected:
        void setLoadState( Document::LoadState );
        void throwFatalError( const QString &message );
        void setCollaborative( bool is_collaborative );
    
    private:
        QPointer<KTextEditor::Document> m_kDocument;
        Document::LoadState m_loadState;
        bool m_isCollaborative;

};

/**
 * @brief Links together the InfTextBuffer and KTextEditor::Document
 */

class KDocumentTextBuffer
    : public QInfinity::AbstractTextBuffer
{
    Q_OBJECT;

    public:
        KDocumentTextBuffer( KTextEditor::Document &kDocument,
            const QString &encoding,
            QObject *parent = 0 );
        ~KDocumentTextBuffer();

        KTextEditor::Document *kDocument() const;
        void onInsertText( unsigned int offset,
            const QInfinity::TextChunk &chunk,
            QInfinity::User *user );
        void onEraseText( unsigned int offset,
            unsigned int length,
            QInfinity::User *user );
        void setUser( QPointer<QInfinity::User> user );

    public Q_SLOTS:
        void joinFailed( GError *error );

    private Q_SLOTS:
        void localTextInserted( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void localTextRemoved( KTextEditor::Document *document,
            const KTextEditor::Range &range );

    private:
        unsigned int cursorToOffset( const KTextEditor::Cursor &cursor );
        KTextEditor::Cursor offsetToCursor( unsigned int offset );

        bool blockLocalInsert;
        bool blockLocalRemove;
        bool blockRemoteInsert;
        bool blockRemoteRemove;
        KTextEditor::Document *m_kDocument;
        QPointer<QInfinity::User> m_user;

};

/**
 * @brief Implementation of Document for InfText infinote plugin.
 */
class InfTextDocument
    : public Document
{
    Q_OBJECT
    
    public:
        /**
         * @brief Create InfTextDocument.
         *
         * Takes ownership of passed session and sessionProxy.
         */
        InfTextDocument( QInfinity::SessionProxy &sessionProxy,
            QInfinity::TextSession &sesion,
            KDocumentTextBuffer &buffer );
        ~InfTextDocument();

    public Q_SLOTS:
        void undo();
        void redo();
    
    private Q_SLOTS:
        void slotSynchronized();
        void slotSynchronizationFailed( GError *gerror );
        void slotJoinFinished( QPointer<QInfinity::User> );
        void slotJoinFailed( GError *gerror );
        void slotViewCreated( KTextEditor::Document *kDoc,
            KTextEditor::View *kView );
    
    private:
        void synchronize();
        void joinSession();
        
        QInfinity::SessionProxy *m_sessionProxy;
        QInfinity::TextSession *m_session;
        KDocumentTextBuffer *m_buffer;
        QPointer<QInfinity::AdoptedUser> m_user;

        // Manage undo/redo
        QList<QAction*> undo_actions;
        QList<QAction*> redo_actions;
        unsigned int insert_count;
        unsigned int undo_count;
    
};

}

#endif

