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

Document::Document( KTextEditor::Document &kDocument,
    QObject *parent )
    : QObject( parent )
    , m_kDocument( &kDocument )
{
    m_kDocument->setParent( this );
}

Document::~Document()
{
}

Document::Type Document::type() const
{
    return KDocument;
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

InfTextDocument::InfTextDocument( KTextEditor::Document &kDocument,
    QPointer<QInfinity::SessionProxy> sessionProxy,
    QObject *parent )
    : Document( kDocument, parent )
    , m_sessionProxy( sessionProxy )
{
    kDocument.setReadWrite( false );
    QInfinity::Session *session = m_sessionProxy->session();
    if( session->status() == QInfinity::Session::Synchronizing )
    {
        kDebug() << "Waiting until synchronization completes.";
        connect( session, SIGNAL(synchronizationComplete()),
            this, SLOT(sessionRunning()) );
    }
    else if( session->status() == QInfinity::Session::Running )
        sessionRunning();
    else
        kDebug() << "Session closed.  Editing disabled.";
}

InfTextDocument::~InfTextDocument()
{
    
}

Document::Type InfTextDocument::type() const
{
    return Document::InfText;
}

void InfTextDocument::sessionRunning()
{
    QInfinity::Session *session = m_sessionProxy->session();
    if( session->type() != QInfinity::Session::Text )
    {
        KMessageBox::error( 0, i18n("Unknown session type"), i18n("Unknown session type") );
        return;
    }
    qDebug() << "Attempting to join session as " << KobbySettings::nickName();
    QInfinity::UserRequest *req = QInfinity::TextSession::joinUser( m_sessionProxy,
        KobbySettings::nickName(),
        0 );
    connect( req, SIGNAL(finished( QPointer<QInfinity::User> )),
        this, SLOT(userJoined( QPointer<QInfinity::User> )) );
    connect( req, SIGNAL(failed( GError* )),
        this, SLOT(userJoinFailed( GError* )) );
}

void InfTextDocument::userJoined( QPointer<QInfinity::User> user )
{
    kDebug() << "User join successfull, enabling editing.";
    m_user = user;
    block_inf_ins_op = false;
    block_inf_del_op = false;
    QPointer<QInfinity::Buffer> buffer = m_sessionProxy->session()->buffer();
    m_textBuffer = dynamic_cast<QInfinity::TextBuffer*>(buffer.data());
    connect( kDocument(), SIGNAL(textInserted( KTextEditor::Document*,
            const KTextEditor::Range )),
        this, SLOT(slotKTextInserted( KTextEditor::Document*,
            const KTextEditor::Range& )) );
    connect( kDocument(), SIGNAL(textRemoved( KTextEditor::Document*,
            const KTextEditor::Range )),
        this, SLOT(slotKTextRemoved( KTextEditor::Document*,
            const KTextEditor::Range& )) );
    connect( m_textBuffer, SIGNAL(textInserted( unsigned int,
            const QInfinity::TextChunk&, QPointer<QInfinity::User> )),
        this, SLOT(slotInfTextInserted(unsigned int,
            const QInfinity::TextChunk, QPointer<QInfinity::User>)) );
    connect( m_textBuffer, SIGNAL(textErased( unsigned int,
            unsigned int, QPointer<QInfinity::User>)),
        this, SLOT(slotInfTextErased(unsigned int, unsigned int,
            QPointer<QInfinity::User>)) );
    kDocument()->setReadWrite( true );
}

void InfTextDocument::userJoinFailed( GError *error )
{
    QString errorMessage = i18n("User join failed!\n");
    errorMessage.append( error->message );
    KMessageBox::error( 0, errorMessage, i18n("User join failed!") );
    deleteLater();
}

void InfTextDocument::slotKTextInserted( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    unsigned int offset = cursorToOffset( range.start() );
    QString text = kDocument()->text( range );
    QInfinity::TextChunk chunk( "UTF-8" );
    chunk.insertText( 0, text.toUtf8(), text.length(), m_user->id() );
    block_inf_ins_op = true;
    m_textBuffer->insertChunk( offset, chunk, m_user );
}

void InfTextDocument::slotKTextRemoved( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    unsigned int offset = cursorToOffset( range.start() );
    unsigned int end = cursorToOffset( range.end() );
    block_inf_del_op = true;
    m_textBuffer->eraseText( offset, end - offset, m_user );
}

void InfTextDocument::slotInfTextInserted( unsigned int offset,
    const QInfinity::TextChunk &textChunk,
    QPointer<QInfinity::User> user )
{
    if( !block_inf_ins_op )
    {
        QByteArray data = textChunk.text();
        kDebug() << "inserting at offset " << offset << " data " << data;
        KTextEditor::Cursor startCursor = offsetToCursor( offset );
        kDebug() << "at line " << startCursor.line() << " column " << startCursor.column();
        QString text = QString::fromUtf8( data );
        if( text[0] == '\n' )
         text = "\n";
        kDocument()->insertText( startCursor, text );
    }
    block_inf_ins_op = false;
}

void InfTextDocument::slotInfTextErased( unsigned int offset,
    unsigned int len,
    QPointer<QInfinity::User> user )
{
    if( !block_inf_del_op )
    {
        KTextEditor::Range range( offsetToCursor( offset ), offsetToCursor( offset + len ) );
        kDocument()->removeText( range );
    }
    block_inf_del_op = false;
}

unsigned int InfTextDocument::cursorToOffset( const KTextEditor::Cursor &cursor )
{
    unsigned int offset = 0;
    int i, cursor_line = cursor.line();
    for( i = 0; i < cursor_line; i++ )
        offset += kDocument()->lineLength( i ) + 1; // Add newline
    offset += cursor.column();
    return offset;
}

KTextEditor::Cursor InfTextDocument::offsetToCursor( unsigned int offset )
{
    int i;
    for( i = 0; offset > kDocument()->lineLength( i ); i++ )
        offset -= kDocument()->lineLength( i ) + 1; // Subtract newline
    return KTextEditor::Cursor( i, offset );
}

}

