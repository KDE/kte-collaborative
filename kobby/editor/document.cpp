#include "document.h"
#include "kobbysettings.h"

#include <libqinfinity/sessionproxy.h>
#include <libqinfinity/session.h>
#include <libqinfinity/textsession.h>
#include <libqinfinity/userrequest.h>
#include <libqinfinity/user.h>
#include <libqinfinity/textbuffer.h>
#include <libqinfinity/textchunk.h>

#include <KTextEditor/Document>
#include <KTextEditor/Range>
#include <KTextEditor/Cursor>
#include <KLocalizedString>
#include <KMessageBox>
#include <KDebug>

namespace Kobby
{

Document::Document( KTextEditor::Document &kDocument )
    : m_kDocument( &kDocument )
{
}

Document::~Document()
{
}

KTextEditor::Document *Document::kDocument() const
{
    return m_kDocument;
}

bool Document::save()
{
    return m_kDocument->documentSave();
}

QString Document::name()
{
    return m_kDocument->documentName();
}

KDocumentTextBuffer::KDocumentTextBuffer( KTextEditor::Document &kDocument,
    const QString &encoding,
    QObject *parent )
    : QInfinity::AbstractTextBuffer( encoding, parent )
    , Document( kDocument )
{
}

KDocumentTextBuffer::~KDocumentTextBuffer()
{
}

void KDocumentTextBuffer::onInsertText( unsigned int offset,
    const QInfinity::TextChunk &chunk,
    QInfinity::User *user )
{
    KTextEditor::Cursor startCursor = offsetToCursor( offset );
    kDocument()->insertText( startCursor, chunk.text() );
}

void KDocumentTextBuffer::onEraseText( unsigned int offset,
    unsigned int length,
    QInfinity::User *user )
{
}

unsigned int KDocumentTextBuffer::cursorToOffset( const KTextEditor::Cursor &cursor )
{
    unsigned int offset = 0;
    int i, cursor_line = cursor.line();
    for( i = 0; i < cursor_line; i++ )
        offset += kDocument()->lineLength( i ) + 1; // Add newline
    offset += cursor.column();
    return offset;
}

KTextEditor::Cursor KDocumentTextBuffer::offsetToCursor( unsigned int offset )
{
    int i;
    for( i = 0; offset > kDocument()->lineLength( i ); i++ )
        offset -= kDocument()->lineLength( i ) + 1; // Subtract newline
    return KTextEditor::Cursor( i, offset );
}

}

