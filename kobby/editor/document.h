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
}

namespace Kobby
{

/**
 * @brief A base class for interacting with Documents.
 */
class Document
{

    public:
        enum Type
        {
            KDocument = 1,
            InfText = 2
        };

        Document( KTextEditor::Document &kDocument );
        virtual ~Document();

        virtual Type type() const;
        KTextEditor::Document *kDocument() const;
        virtual bool save();
        virtual QString name();
    
    private:
        KTextEditor::Document *m_kDocument;

};

class InfTextDocument
    : public QInfinity::AbstractTextBuffer
    , public Document
{
    Q_OBJECT;

    public:
        InfTextDocument( KTextEditor::Document &kDocument,
            QPointer<QInfinity::SessionProxy> sesisonProxy,
            QObject *parent = 0 );
        ~InfTextDocument();

        Document::Type type() const;

    private Q_SLOTS:
        void sessionRunning();
        void userJoined( QPointer<QInfinity::User> user );
        void userJoinFailed( GError *error );
        void slotKTextInserted( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void slotKTextRemoved( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void slotInfTextInserted( unsigned int offset,
            const QInfinity::TextChunk &textChunk,
            QPointer<QInfinity::User> user );
        void slotInfTextErased( unsigned int offset,
            unsigned int len, QPointer<QInfinity::User> user );

    private:
        unsigned int cursorToOffset( const KTextEditor::Cursor &cursor );
        KTextEditor::Cursor offsetToCursor( unsigned int offset );

        QPointer<QInfinity::SessionProxy> m_sessionProxy;
        QPointer<QInfinity::User> m_user;
        QPointer<QInfinity::TextBuffer> m_textBuffer;
        bool block_inf_ins_op;
        bool block_inf_del_op;

};

}

#endif

