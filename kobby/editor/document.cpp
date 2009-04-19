#include "document.h"
#include "kobbysettings.h"

#include <libqinfinity/sessionproxy.h>
#include <libqinfinity/session.h>
#include <libqinfinity/textsession.h>
#include <libqinfinity/userrequest.h>
#include <libqinfinity/user.h>
#include <libqinfinity/textbuffer.h>
#include <libqinfinity/textchunk.h>
#include <libqinfinity/browseriter.h>

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
    , blockLocalInsert( false )
    , blockLocalRemove( false )
    , blockRemoteInsert( false )
    , blockRemoteRemove( false )
{
    connect( &kDocument, SIGNAL(textInserted(KTextEditor::Document*, const KTextEditor::Range&)),
        this, SLOT(localTextInserted(KTextEditor::Document*, const KTextEditor::Range&)) );
    connect( &kDocument, SIGNAL(textRemoved(KTextEditor::Document*, const KTextEditor::Range&)),
        this, SLOT(localTextRemoved(KTextEditor::Document*, const KTextEditor::Range&)) );
}

KDocumentTextBuffer::~KDocumentTextBuffer()
{
}

void KDocumentTextBuffer::setName( const QString &name )
{
    m_name = name;
}

void KDocumentTextBuffer::onInsertText( unsigned int offset,
    const QInfinity::TextChunk &chunk,
    QInfinity::User *user )
{
    if( !blockRemoteInsert )
    {
        KTextEditor::Cursor startCursor = offsetToCursor( offset );
        blockLocalInsert = true;
        kDocument()->insertText( startCursor, chunk.text() );
    }
    else
        blockRemoteInsert = false;
}

void KDocumentTextBuffer::onEraseText( unsigned int offset,
    unsigned int length,
    QInfinity::User *user )
{
    if( !blockRemoteRemove )
    {
        KTextEditor::Cursor startCursor = offsetToCursor( offset );
        KTextEditor::Cursor endCursor = offsetToCursor( offset+length );
        blockLocalRemove = true;
        kDocument()->removeText( KTextEditor::Range(startCursor, endCursor) );
    }
    else
        blockRemoteRemove = false;
}

QString KDocumentTextBuffer::name()
{
    return m_name;
}

void KDocumentTextBuffer::localTextInserted( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    unsigned int offset;
    if( !blockLocalInsert && m_user )
    {
        offset = cursorToOffset( range.start() );
        QInfinity::TextChunk chunk( "UTF-8" );
        QString text = kDocument()->text( range );
        chunk.insertText( 0, text.toUtf8(), text.length(), m_user->id() );
        blockRemoteInsert = true;
        insertChunk( offset, chunk, m_user );
    }
    else
        blockLocalInsert = false;
}

void KDocumentTextBuffer::localTextRemoved( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    if( !blockLocalRemove && m_user )
    {
        unsigned int offset = cursorToOffset( range.start() );
        unsigned int end = cursorToOffset( range.end() );
        blockRemoteRemove = true;
        eraseText( offset, end-offset, m_user );
    }
    else
        blockLocalRemove = false;
}

void KDocumentTextBuffer::setUser( QPointer<QInfinity::User> user )
{
    m_user = user;
    kDocument()->setReadWrite( true );
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

