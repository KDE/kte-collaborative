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
#include "kobbycommon_export.h"

#include <libqinfinity/abstracttextbuffer.h>

#include <QObject>
#include <QPointer>
#include <QStack>
#include <QTimer>
#include <KUrl>
#include <KTextEditor/Document>

typedef struct _GError GError;

class QString;
class QAction;

namespace KTextEditor
{
    class Document;
    class Range;
    class Cursor;
    class View;
    class BufferInterface;
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
    class UndoGrouping;
}

namespace Kobby
{

int countUnicodeCharacters(const QString& str);

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
class KOBBYCOMMON_EXPORT Document
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
        
        enum DocType
        {
            KDocument = 1,
            InfText = 2
        };

        Document( KTextEditor::Document* kDocument );
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
        virtual QString name() const;
        
        /**
         * @brief State of document loading.
         * 
         * Initial state is Unloaded.
         */
        Document::LoadState loadState() const;

        virtual int type() const;

        virtual void leave() { }

        bool isDirty();

        void throwFatalError( const QString &message );

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
         * An empty message should not result in a user-visible action
         * (e.g. a message box with the error).
         */
        void fatalError( Document *document,
            QString message );
    
    protected:
        void setLoadState( Document::LoadState );

    private Q_SLOTS:
        void textChanged( KTextEditor::Document *document );
        void documentSaved( KTextEditor::Document *document,
            bool saveAs );

    private:
        QPointer<KTextEditor::Document> m_kDocument;
        Document::LoadState m_loadState;
        bool m_dirty;

};

/**
 * @brief Links together the InfTextBuffer and KTextEditor::Document
 *
 * The KDocumentTextBuffer ties together remote and local insertion
 * and removal operations.  It is also responsible for maintaining
 * undo/redo stats (insertionCount and undoCount).
 */
class KOBBYCOMMON_EXPORT KDocumentTextBuffer
    : public QInfinity::AbstractTextBuffer
{
    Q_OBJECT

    public:
        KDocumentTextBuffer( KTextEditor::Document* kDocument,
            const QString &encoding,
            QObject *parent = 0 );
        ~KDocumentTextBuffer();

        KTextEditor::Document* kDocument() const;
        void onInsertText( unsigned int offset,
            const QInfinity::TextChunk &chunk,
            QInfinity::User *user );
        void onEraseText( unsigned int offset,
            unsigned int length,
            QInfinity::User *user );
        void setUser( QPointer<QInfinity::User> user );
        bool hasUser() const;
        QInfinity::User* user() const;
        void setSession( QInfinity::Session* session );

        void updateUndoRedoActions();

        void checkConsistency();
        void checkLineEndings();
        void shutdown();

    Q_SIGNALS:
        void canUndo( bool enable );
        void canRedo( bool enable );
        void fatalError( const QString &message );
        // Emitted when a user except you changes text
        void remoteChangedText( const KTextEditor::Range& range, QInfinity::User* user, bool removal );
        // Emitted when you changed text
        void localChangedText( const KTextEditor::Range& range, QInfinity::User* user, bool removal );

    public Q_SLOTS:
        void nextUndoStep();

    private Q_SLOTS:
        void localTextInserted( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void localTextRemoved( KTextEditor::Document *document,
            const KTextEditor::Range &range, const QString& oldText );
        void replaceLineEndings();

    private:
        // All offsets are in unicode code points, all cursors are in utf-16 surrogates.
        KTextEditor::Cursor offsetRelativeTo_kte(const KTextEditor::Cursor& cursor,
                                                 const unsigned int offset);
        KTextEditor::Cursor offsetToCursor_kte( unsigned int offset );
        unsigned int cursorToOffset_kte( const KTextEditor::Cursor &cursor );
        void textOpPerformed();
        void resetUndoRedo();

        bool blockRemoteInsert;
        bool blockRemoteRemove;
        KTextEditor::Document *m_kDocument;
        QPointer<QInfinity::User> m_user;

        // Undo/Redo management
        QInfinity::Session* m_session;
        QTimer m_undoTimer;
        QPointer<QInfinity::UndoGrouping> m_undoGrouping;

        bool m_aboutToClose;

        friend class InfTextDocument;
};

class DocumentReopenHelper : public QObject {
Q_OBJECT
public:
    DocumentReopenHelper(KUrl url, KTextEditor::Document* document)
        : QObject()
        , oldUrl(url)
        , document(document) {};

public slots:
    void reopen() {
        document->openUrl(oldUrl);
        deleteLater();
    };

private:
    KUrl oldUrl;
    KTextEditor::Document* document;
};

/**
 * @brief Implementation of Document for InfText infinote plugin.
 *
 * Ties local operations/acitons into QInfinity::Session operations.
 */
class KOBBYCOMMON_EXPORT InfTextDocument
    : public Document
{
    Q_OBJECT
    
    public:
        /**
         * @brief Create InfTextDocument.
         *
         * Takes ownership of passed session and sessionProxy.
         */
        InfTextDocument( QInfinity::SessionProxy* sessionProxy,
            QInfinity::TextSession* session,
            KDocumentTextBuffer* buffer,
            const QString &name );
        ~InfTextDocument();

        QString name() const;
        int type() const;
        QPointer<QInfinity::TextSession> infSession() const;
        void leave();
        
    public Q_SLOTS:
        void undo();
        void redo();
    
    private Q_SLOTS:
        void slotSynchronized();
        void slotSynchronizationFailed( GError *gerror );
        void slotJoinFinished( QPointer<QInfinity::User> );
        void slotJoinFailed( GError *gerror );
        void slotViewCreated( KTextEditor::Document *doc,
            KTextEditor::View *view );
        void slotCanUndo( bool enable );
        void slotCanRedo( bool enable );
        void joinSession(const QString& forceUserName = QString());
    
    private:
        void synchronize();
        void retryJoin(const QString& message);
        
        QPointer<QInfinity::SessionProxy> m_sessionProxy;
        QPointer<QInfinity::TextSession> m_session;
        KDocumentTextBuffer *m_buffer;
        QPointer<QInfinity::AdoptedUser> m_user;
        QString m_name;

        // Undo/Redo actions
        QList<QAction*> undoActions;
        QList<QAction*> redoActions;
};

}

#endif

