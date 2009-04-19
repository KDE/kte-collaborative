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
        KTextEditor::Document *m_kDocument;

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

        void onInsertText( unsigned int offset,
            const QInfinity::TextChunk &chunk,
            QInfinity::User *user );

        void onEraseText( unsigned int offset,
            unsigned int length,
            QInfinity::User *user );

    private:
        unsigned int cursorToOffset( const KTextEditor::Cursor &cursor );
        KTextEditor::Cursor offsetToCursor( unsigned int offset );

};

}

#endif

