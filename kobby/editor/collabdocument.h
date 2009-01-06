#ifndef KOBBY_COLLABDOCUMENT_H
#define KOBBY_COLLABDOCUMENT_H

#include <QObject>

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
    class ClientSessionProxy;
    class XmlConnection;
}

namespace KTextEditor
{
    class Document;
    class Range;
    class Cursor;
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
        CollabDocument( Glib::RefPtr<Infinity::ClientSessionProxy> &sessionProxy,
            KTextEditor::Document &document,
            QObject *parent = 0 );
        ~CollabDocument();

        KTextEditor::Document *kDocument() const;

    private Q_SLOTS:
        void slotLocalTextInserted( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void slotInsertText( unsigned int pos,
            Infinity::TextChunk textChunk,
            Infinity::User *user );

    private:
        void setupSessionActions();
        void setupDocumentActions();
        unsigned int cursorToPos( const KTextEditor::Cursor &cursor, KTextEditor::Document &document );
        void sessionSynchronizationComplete( Infinity::XmlConnection *connection );
        void sessionStatusChanged();

        Infinity::TextBuffer *m_textBuffer;
        Infinity::Session *m_infSession;
        KTextEditor::Document *m_kDocument;
        Glib::RefPtr<Infinity::ClientSessionProxy> *m_sessionProxy;

};

}

#endif

