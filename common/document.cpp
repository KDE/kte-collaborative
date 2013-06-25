/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 * Copyright 2009  Ryan Kavanagh <ryanakca@kubuntu.org>
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

#include "document.h"
#include "documentmodel.h"

#include <libqinfinity/sessionproxy.h>
#include <libqinfinity/session.h>
#include <libqinfinity/textsession.h>
#include <libqinfinity/userrequest.h>
#include <libqinfinity/adopteduser.h>
#include <libqinfinity/textbuffer.h>
#include <libqinfinity/textchunk.h>
#include <libqinfinity/browseriter.h>

#include <KTextEditor/Document>
#include <KTextEditor/Range>
#include <KTextEditor/Cursor>
#include <KTextEditor/View>
#include <KLocalizedString>
#include <KMessageBox>
#include <KDebug>

#include <QString>
#include <QTextCodec>
#include <QTextEncoder>
#include <QTime>
#include <QAction>

#ifdef KTEXTEDITOR_HAS_BUFFER_IFACE
#include <ktexteditor/bufferinterface.h>
#endif

namespace Kobby
{

Document::Document( KTextEditor::Document* kDocument )
    : m_kDocument( kDocument )
    , m_loadState( Document::Unloaded )
    , m_dirty( false )
{
    m_kDocument->setParent( 0 );
    connect( m_kDocument, SIGNAL(textChanged( KTextEditor::Document* )),
        this, SLOT(textChanged( KTextEditor::Document* )) );
    connect( m_kDocument, SIGNAL(documentSavedOrUploaded( KTextEditor::Document*,
            bool )),
        this, SLOT(documentSaved( KTextEditor::Document*, bool )) );
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

QString Document::name() const
{
    return m_kDocument->documentName();
}

Document::LoadState Document::loadState() const
{
    return m_loadState;
}

int Document::type() const
{
    return Document::KDocument;
}

bool Document::isDirty()
{
    return m_dirty;
}

void Document::setLoadState( Document::LoadState state )
{
    if( state != LoadState() )
    {
        m_loadState = state;
        emit( loadStateChanged( this, state ) );
        if( state == Document::Complete )
            emit( loadingComplete( this ) );
    }
}

void Document::textChanged( KTextEditor::Document *doc )
{
    m_dirty = true;
}

void Document::documentSaved( KTextEditor::Document *doc,
    bool saveAs )
{
    m_dirty = false;
}

void Document::throwFatalError( const QString &message )
{
    emit( fatalError( this, message ) );
}

KDocumentTextBuffer::KDocumentTextBuffer( KTextEditor::Document* kDocument,
    const QString &encoding,
    QObject *parent )
    : QInfinity::AbstractTextBuffer( encoding, parent )
    , blockRemoteInsert( false )
    , blockRemoteRemove( false )
#ifdef KTEXTEDITOR_HAS_BUFFER_IFACE
    , m_bufferInterface( qobject_cast<KTextEditor::BufferInterface*>(kDocument) )
#endif
    , m_kDocument( kDocument )
    , m_insertCount( 0 )
    , m_undoCount( 0 )
    , undo_lock( false )
    , redo_lock( false )
{
    kDebug() << "new text buffer for document" << kDocument;
    connect( kDocument, SIGNAL(textInserted(KTextEditor::Document*, const KTextEditor::Range&)),
        this, SLOT(localTextInserted(KTextEditor::Document*, const KTextEditor::Range&)) );
    connect( kDocument, SIGNAL(textRemoved(KTextEditor::Document*, const KTextEditor::Range&)),
        this, SLOT(localTextRemoved(KTextEditor::Document*, const KTextEditor::Range&)) );
}

KDocumentTextBuffer::~KDocumentTextBuffer()
{
}

KTextEditor::Document *KDocumentTextBuffer::kDocument() const
{
    return m_kDocument;
}

Document *KDocumentTextBuffer::document()
{
    return DocumentModel::instance()->documentFromKDoc(*kDocument());
}

void KDocumentTextBuffer::onInsertText( unsigned int offset,
    const QInfinity::TextChunk &chunk,
    QInfinity::User *user )
{
    kDebug() << "REMOTE INSERT TEXT offset" << offset << chunk.text() << kDocument()
             << "(" << chunk.length() << " chars )" << "[blocked:" << blockRemoteInsert << "]";

    if( !blockRemoteInsert )
    {
        KTextEditor::Cursor startCursor = offsetToCursor_remote( offset );
        QString str = codec()->toUnicode( chunk.text() );
        kDocument()->blockSignals(true);
#ifdef KTEXTEDITOR_HAS_BUFFER_IFACE
        // The compile-time check just verifies that the interface is present.
        // This does not guarantee that it is supported by the KTE implementation used here.
        if ( m_bufferInterface ) {
            m_bufferInterface->insertTextRaw(startCursor.line(), startCursor.column(), str);
        }
#else
        if ( false ) { }
#endif
        else {
            kWarning() << "Text editor does not support the Buffer interface!";
            kDocument()->insertText( startCursor, str );
        }
        kDocument()->blockSignals(false);
        emit remoteChangedText(KTextEditor::Range(startCursor, offsetToCursor_remote(offset+chunk.length())), user, false);
    }
    else
        blockRemoteInsert = false;
}

void KDocumentTextBuffer::onEraseText( unsigned int offset,
    unsigned int length,
    QInfinity::User *user )
{
    kDebug() << "REMOTE ERASE TEXT len" << length << "offset" << offset;

    if( !blockRemoteRemove )
    {
        KTextEditor::Cursor startCursor = offsetToCursor_remote( offset );
        KTextEditor::Cursor endCursor = offsetToCursor_remote( offset+length );
        KTextEditor::Range range = KTextEditor::Range(startCursor, endCursor);
        kDocument()->blockSignals(true);
#ifdef KTEXTEDITOR_HAS_BUFFER_IFACE
        // see onInsertText
        if ( m_bufferInterface ) {
            m_bufferInterface->removeTextRaw(startCursor.line(), startCursor.column(),
                                             endCursor.line(), endCursor.column());
        }
#else
        if ( false ) { }
#endif
        else {
            kWarning() << "Text editor does not support the Buffer interface!";
            kDocument()->removeText( range );
        }
        kDocument()->blockSignals(false);
        emit remoteChangedText(range, user, true);
    }
    else
        blockRemoteRemove = false;
}

void KDocumentTextBuffer::joinFailed( GError *error )
{
    QString errorString = i18n("Joining failed: ");
    errorString.append( error->message );
    KMessageBox::error( 0, errorString, i18n("Joining Failed") );
}

void KDocumentTextBuffer::localTextInserted( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    kDebug() << "local text inserted" << kDocument() << "(range" << range << ")" << m_user;
    emit localChangedText(range, user(), false);
    Q_UNUSED(document)

    textOpPerformed();
    unsigned int offset;
    if( !m_user.isNull() )
    {
        offset = cursorToOffset_local( range.start() );
        QInfinity::TextChunk chunk( encoding() );
        QString text = kDocument()->text( range );
        if( encoder() )
        {
            if( text.isEmpty() )
            {
                kDebug() << "Skipping empty insert.";
            }
            else
            {
                QByteArray encodedText = codec()->fromUnicode( text );
                if( encodedText.size() == 0 )
                {
                    kDebug() << "Got empty encoded text from non empty string "
                                "Skipping insertion";
                    this->document()->throwFatalError( i18n("Document state compromised") );
                }
                else
                {
                    chunk.insertText( 0, encodedText, text.length(), m_user->id() );
                    blockRemoteInsert = true;
                    kDebug() << "inserting chunk of size" << chunk.length() << "into local buffer";
                    insertChunk( offset, chunk, m_user );
                }
            }
        }
        else
            kDebug() << "No encoder for text codec.";
    }
    else
        kDebug() << "Could not insert text: No local user set.";
}

void KDocumentTextBuffer::localTextRemoved( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    kDebug() << "local text removed:" << kDocument() << range;
    emit localChangedText(range, user(), true);

    Q_UNUSED(document)
    unsigned int offset;
    unsigned int end;
    unsigned int len;
    KTextEditor::Range chkRange;

    textOpPerformed();
    if( !m_user.isNull() )
    {
        offset = cursorToOffset_local( range.start() );
        end = cursorToOffset_local( range.end() );
        len = end - offset;
        blockRemoteRemove = true;
        kDebug() << "ERASING TEXT with len" << len << "offset" << offset;
        if( len > 0 )
            eraseText( offset, len, m_user );
        else
            kDebug() << "0 legth delete operation. Skipping.";
    }
    else
        kDebug() << "Could not remove text: No local user set.";
}

void KDocumentTextBuffer::setUser( QPointer<QInfinity::User> user )
{
    m_user = user;
}

void KDocumentTextBuffer::resetUndoRedo()
{
    kDebug() << "reset";
    if( m_insertCount )
    {
        m_insertCount = 0;
        emit( canUndo( false ) );
    }
    if( m_undoCount )
    {
        m_undoCount = 0;
        emit( canRedo( false ) );
    }
}

void KDocumentTextBuffer::performingUndo()
{
    undo_lock = true;
    if( m_insertCount )
    {
        m_insertCount--;
    }
    if( !m_insertCount )
    {
        emit( canUndo( false ) );
    }
    if( m_undoCount )
        m_undoCount++;
    else
    {
        m_undoCount++;
        emit( canRedo( true ) );
    }
}

void KDocumentTextBuffer::performingRedo()
{
    redo_lock = true;
    if( !m_insertCount )
    {
        m_insertCount++;
        emit( canUndo( true ) );
    }
    else
        m_insertCount++;
    if( m_undoCount )
    {
        m_undoCount--;
    }
    if( !m_undoCount )
    {
        emit( canRedo( false ) );
    }
}

unsigned int KDocumentTextBuffer::insertCount() const
{
    return m_insertCount;
}

unsigned int KDocumentTextBuffer::undoCount() const
{
    return m_undoCount;
}

unsigned int KDocumentTextBuffer::cursorToOffset_local( const KTextEditor::Cursor &cursor )
{
    unsigned int offset = 0;
    int i, cursor_line = cursor.line();
    QList<QByteArray> lines = slice(0, length())->text().split('\n');
    for( i = 0; i < cursor_line; i++ )
        offset += codec()->toUnicode(lines.at(i)).length() + 1; // Add newline
    offset += cursor.column();
    kDebug() << "cursor:" << cursor << "-> offset:" << offset;
    return offset;
}

KTextEditor::Cursor KDocumentTextBuffer::offsetToCursor_local( unsigned int offset )
{
    int soff = 0;
    if( offset > 0 )
        soff = offset;
    int i;
    QList<QByteArray> lines = slice(0, length())->text().split('\n');
    for( i = 0; soff > codec()->toUnicode(lines.at(i)).length(); i++ ) {
        soff -= codec()->toUnicode(lines.at(i)).length() + 1; // Subtract newline
        if ( i == lines.size() - 1 ) {
            kWarning() << "oops, invalid offset?";
            break;
        }
    }
    return KTextEditor::Cursor( i, soff );
}

unsigned int KDocumentTextBuffer::cursorToOffset_remote( const KTextEditor::Cursor &cursor )
{
    unsigned int offset = 0;
    int i, cursor_line = cursor.line();
    for( i = 0; i < kDocument()->lineLength(i); i++ )
        offset += kDocument()->lineLength(i) + 1; // Add newline
    offset += cursor.column();
    return offset;
}

KTextEditor::Cursor KDocumentTextBuffer::offsetToCursor_remote( unsigned int offset )
{
    int soff = 0;
    if( offset > 0 )
        soff = offset;
    int i;
    for( i = 0; soff > kDocument()->lineLength(i); i++ ) {
        soff -= kDocument()->lineLength(i) + 1; // Subtract newline
        if ( kDocument()->lineLength(i) == -1 ) {
            kWarning() << "oops, invalid offset?";
            break;
        }
    }
    return KTextEditor::Cursor( i, soff );
}

void KDocumentTextBuffer::textOpPerformed()
{
    if( undo_lock )
    {
        undo_lock = false;
    }
    else if( redo_lock )
    {
        redo_lock = false;
    }
    else
    {
        if( !m_insertCount )
        {
            m_insertCount++;
            emit( canUndo( true ) );
        }
        else
            m_insertCount++;
        if( m_undoCount )
        {
            m_undoCount = 0;
            emit( canRedo( false ) );
        }
    }
}

/* Accepting the session and buffer as parameters, although we
   could obtain them from the session proxy, ensures some type
   safety. */
InfTextDocument::InfTextDocument( QInfinity::SessionProxy* proxy,
    QInfinity::TextSession* session,
    KDocumentTextBuffer* buffer,
    const QString &name )
    : Document( buffer->kDocument() )
    , m_sessionProxy( proxy )
    , m_session( session )
    , m_buffer( buffer )
    , m_name( name )
    , m_user( 0 )
{
    kDebug() << "new infTextDocument for url" << kDocument()->url();
    m_session->setParent( this );
    m_sessionProxy->setParent( this );
    connect( kDocument(), SIGNAL(viewCreated( KTextEditor::Document*, KTextEditor::View* )),
        this, SLOT(slotViewCreated( KTextEditor::Document*, KTextEditor::View* )) );
    connect( buffer, SIGNAL(canUndo( bool )),
        this, SLOT(slotCanUndo( bool )) );
    connect( buffer, SIGNAL(canRedo( bool )),
        this, SLOT(slotCanRedo( bool )) );
    synchronize();
}

InfTextDocument::~InfTextDocument()
{
    m_session->close();
}

QInfinity::User* KDocumentTextBuffer::user() const
{
    return m_user;
}

QString InfTextDocument::name() const
{
    return m_name;
}

int InfTextDocument::type() const
{
    return Document::InfText;
}

QPointer<QInfinity::TextSession> InfTextDocument::infSession() const
{
    return m_session;
}

void InfTextDocument::leave()
{
    if( m_user )
        m_session->setUserStatus( *m_user, QInfinity::User::Unavailable );
}

void InfTextDocument::undo()
{
    m_buffer->performingUndo();
    if( m_user )
        m_session->undo( *m_user, 1 );
}

void InfTextDocument::redo()
{
    m_buffer->performingRedo();
    if( m_user )
        m_session->redo( *m_user, 1 );
}

void InfTextDocument::slotSynchronized()
{
    setLoadState( Document::SynchronizationComplete );
    joinSession();
    m_buffer->resetUndoRedo();
}

void InfTextDocument::slotSynchronizationFailed( GError *gerror )
{
    QString emsg = i18n( "Synchronization Failed: " );
    emsg.append( gerror->message );
    throwFatalError( emsg );
}

bool KDocumentTextBuffer::hasUser() const
{
    if ( m_user) {
        kDebug() << "user" << m_user->name() << "status:" << m_user->status();
    }
    return m_user != 0;
}

void InfTextDocument::slotJoinFinished( QPointer<QInfinity::User> user )
{
    m_buffer->setUser( user );
    m_user = dynamic_cast<QInfinity::AdoptedUser*>(user.data());
    setLoadState( Document::JoiningComplete );
    setLoadState( Document::Complete );
    kDebug() << "Join successful, user" << user->name() << "now online";
}

void InfTextDocument::slotJoinFailed( GError *gerror )
{
    QString emsg = i18n( "Could not join session: " );
    emsg.append( gerror->message );
    throwFatalError( emsg );
    kDebug() << "Join failed: " << emsg;
}

void InfTextDocument::slotViewCreated( KTextEditor::Document *doc,
    KTextEditor::View *view )
{
    Q_UNUSED(doc)
    // HACK: Steal the undo/redo actions
    QAction *act = view->action( "edit_undo" );
    if( act )
    {
        undoActions.append( act );
        act->disconnect();
        connect( act, SIGNAL(triggered(bool)),
            this, SLOT(undo()) );
    }
    act = view->action( "edit_redo" );
    if( act )
    {
        redoActions.append( act );
        act->disconnect();
        connect( act, SIGNAL(triggered(bool)),
            this, SLOT(redo()) );
    }
}

void InfTextDocument::slotCanUndo( bool enable )
{
    kDebug() << "set undo:" << enable;
    QAction *act;
    foreach( act, undoActions )
    {
        act->setEnabled( enable );
    }
}

void InfTextDocument::slotCanRedo( bool enable )
{
    QAction *act;
    foreach( act, redoActions )
    {
        act->setEnabled( enable );
    }
}

void InfTextDocument::synchronize()
{
    kDebug() << "synchronizing document";
    if( m_session->status() == QInfinity::Session::Running )
        slotSynchronized();
    else if( m_session->status() == QInfinity::Session::Synchronizing )
    {
        setLoadState( Document::Synchronizing );
        connect( m_session, SIGNAL(synchronizationComplete()),
            this, SLOT(slotSynchronized()) );
        connect( m_session, SIGNAL(synchronizationFailed( GError* )),
            this, SLOT(slotSynchronizationFailed( GError* )) );
    }
}

void InfTextDocument::joinSession()
{
    if( m_session->status() == QInfinity::Session::Running )
    {
        // We dont want this being called again
        disconnect( this, SLOT(joinSession()) );
        
        setLoadState( Document::Joining );
        QString userName;
        if ( ! kDocument()->url().userName().isEmpty() ) {
            userName = kDocument()->url().userName();
        }
        else {
            userName = "UnnamedUser_" + QString::number(QTime::currentTime().second());
        }
        kDebug() << "requesting join of user" << userName;
        QInfinity::UserRequest *req = QInfinity::TextSession::joinUser( m_sessionProxy,
            *m_session,
            userName,
            10 );
        connect( req, SIGNAL(finished(QPointer<QInfinity::User>)),
            this, SLOT(slotJoinFinished(QPointer<QInfinity::User>)) );
        connect( req, SIGNAL(failed(GError*)),
            this, SLOT(slotJoinFailed(GError*)) );
    }
    else
        connect( m_session, SIGNAL(statusChanged()),
            this, SLOT(joinSession()), Qt::UniqueConnection );
}

}

