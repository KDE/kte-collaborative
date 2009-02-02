#ifndef KOBBY_COLLABDOCUMENT_H
#define KOBBY_COLLABDOCUMENT_H

#include <libqinfinitymm/session.h>

#include <QObject>
#include <QTextStream>

#include <glibmm/refptr.h>

namespace QInfinity
{
    class Document;
}

namespace Infinity
{
    class User;
    class TextChunk;
    class TextBuffer;
    class Session;
    class ClientUserRequest;
    class ClientSessionProxy;
    class XmlConnection;
}

namespace KTextEditor
{
    class Document;
    class Range;
    class Cursor;
    class View;
}

namespace Kobby
{

/**
 * @brief Connects Infinity::TextBuffer with KTextEditor::Document
 *
 * The CollabDocument is responsible for connecting the signals/slots to insert/remove
 * text from Infinity::TextBuffer and KTextEditor::Document.  It also handles waiting for
 * the synchronizationComplete signal from Infinity::Session, and maintains a reference to
 * a sessionProxy if instantiated from one.
 */
class CollabDocument
    : public QObject
{
    Q_OBJECT

    public:
        CollabDocument(  QInfinity::Session &session,
            KTextEditor::Document &document,
            QObject *parent = 0 );
        ~CollabDocument();

        KTextEditor::Document *kDocument() const;
        QInfinity::Session &session() const;

    private Q_SLOTS:
        void slotLocalTextInserted( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void slotLocalEraseText( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void slotRemoteInsertText( unsigned int pos,
            Infinity::TextChunk textChunk,
            Infinity::User *user );
        void slotRemoteEraseText( unsigned int pos,
            unsigned int len,
            Infinity::User *user );
        void slotLocalTextChanged( KTextEditor::Document *document,
            const KTextEditor::Range &oldRange,
            const KTextEditor::Range &newRange );
        void slotSynchronizationComplete();

    private:
        void setupSessionActions();
        void setupDocumentActions();
        unsigned int cursorToPos( const KTextEditor::Cursor &cursor, KTextEditor::Document &document );
        void userRequestFinished( Infinity::User *user );
        void joinUser();
        KTextEditor::Cursor posToCursor( int pos ) const;
        int cursorToPos( KTextEditor::Cursor cursor ) const;
        void debugInsert( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void debugRemove( KTextEditor::Document *document,
            const KTextEditor::Range &range );


        QInfinity::Session *m_session;
        Infinity::TextBuffer *m_textBuffer;
        Infinity::Session *m_infSession;
        KTextEditor::Document *m_kDocument;
        Glib::RefPtr<Infinity::ClientSessionProxy> m_sessionProxy;
        Glib::RefPtr<Infinity::ClientUserRequest> userRequest;
        Infinity::User *localUser;
        QTextStream textStream;

};

}

#endif

