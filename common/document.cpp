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

void Document::textChanged( KTextEditor::Document */*doc*/ )
{
    m_dirty = true;
}

void Document::documentSaved( KTextEditor::Document */*doc*/,
    bool /*saveAs*/ )
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
    , m_kDocument( kDocument )
    , m_changeCount( 0 )
    , m_undoCount( 0 )
    , undo_lock( false )
    , redo_lock( false )
{
    kDebug() << "new text buffer for document" << kDocument;
    connect( kDocument, SIGNAL(textInserted(KTextEditor::Document*, const KTextEditor::Range&)),
        this, SLOT(localTextInserted(KTextEditor::Document*, const KTextEditor::Range&)) );
    connect( kDocument, SIGNAL(textRemoved(KTextEditor::Document*, const KTextEditor::Range&, const QString&)),
        this, SLOT(localTextRemoved(KTextEditor::Document*, const KTextEditor::Range&, const QString&)) );
}

KDocumentTextBuffer::~KDocumentTextBuffer()
{
}

KTextEditor::Document *KDocumentTextBuffer::kDocument() const
{
    return m_kDocument;
}

/**
 * @brief Makes a KPart read-write while it exists and restores the previous state on destruction.
 */
class ReadWriteTransaction {
public:
    ReadWriteTransaction(KTextEditor::Document* document) {
        m_document = document;
        m_wasReadWrite = document->isReadWrite();
        document->setReadWrite(true);
    };

    ~ReadWriteTransaction() {
        m_document->setReadWrite(m_wasReadWrite);
    };

private:
    KTextEditor::Document* m_document;
    bool m_wasReadWrite;
};

void KDocumentTextBuffer::onInsertText( unsigned int offset,
    const QInfinity::TextChunk &chunk,
    QInfinity::User *user )
{

    if( !blockRemoteInsert )
    {
        kDebug() << "REMOTE INSERT TEXT offset" << offset << chunk.text() << kDocument()
                 << "(" << chunk.length() << " chars )" << kDocument()->url();
        KTextEditor::Cursor startCursor = offsetToCursor_inf( offset );
        QString str = codec()->toUnicode( chunk.text() );
        ReadWriteTransaction transaction(kDocument());
#ifdef KTEXTEDITOR_HAS_BUFFER_IFACE
        // The compile-time check just verifies that the interface is present.
        // This does not guarantee that it is supported by the KTE implementation used here.
        if ( KTextEditor::BufferInterface* iface = qobject_cast<KTextEditor::BufferInterface*>(kDocument()) ) {
            kDebug() << "buffer insert start vvvvvv";
            iface->insertTextRaw(startCursor.line(), startCursor.column(), str);
            kDebug() << "buffer insert end   ^^^^^^";
        }
#else
        if ( false ) { }
#endif
        else {
            kWarning() << "Text editor does not support the Buffer interface!";
            kDocument()->blockSignals(true);
            kDocument()->insertText( startCursor, str );
            kDocument()->blockSignals(false);
        }
        emit remoteChangedText(KTextEditor::Range(startCursor, offsetToCursor_inf(offset+chunk.length())), user, false);
        emit canUndo(m_changeCount > 0);
        emit canRedo(m_undoCount > 0);
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
        kDebug() << "REMOTE ERASE TEXT len" << length << "offset" << offset << kDocument()->url();
        KTextEditor::Cursor startCursor = offsetToCursor_kte( offset );
        KTextEditor::Cursor endCursor = offsetToCursor_kte( offset+length );
        KTextEditor::Range range = KTextEditor::Range(startCursor, endCursor);
        ReadWriteTransaction transaction(kDocument());
#ifdef KTEXTEDITOR_HAS_BUFFER_IFACE
        // see onInsertText
        if ( KTextEditor::BufferInterface* iface = qobject_cast<KTextEditor::BufferInterface*>(kDocument()) ) {
            kDebug() << "buffer erase start vvvvvv";
            iface->removeTextRaw(startCursor.line(), startCursor.column(),
                                 endCursor.line(), endCursor.column());
            kDebug() << "buffer erase end   ^^^^^^";
        }
#else
        if ( false ) { }
#endif
        else {
            kWarning() << "Text editor does not support the Buffer interface!";
            kDocument()->blockSignals(true);
            kDocument()->removeText( range );
            kDocument()->blockSignals(false);
        }
        emit remoteChangedText(range, user, true);
        emit canUndo(m_changeCount > 0);
        emit canRedo(m_undoCount > 0);
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
        offset = cursorToOffset_kte( range.start() );
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
                    Q_ASSERT(false);
                }
                else
                {
                    chunk.insertText( 0, encodedText, text.length(), m_user->id() );
                    blockRemoteInsert = true;
                    kDebug() << "inserting chunk of size" << chunk.length() << "into local buffer" << kDocument()->url();
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
    const KTextEditor::Range &range, const QString& oldText )
{
    kDebug() << "local text removed:" << kDocument() << range;
    emit localChangedText(range, user(), true);

    Q_UNUSED(document)
    unsigned int offset;
    unsigned int len;

    textOpPerformed();
    if( !m_user.isNull() )
    {
        offset = cursorToOffset_kte( range.start() );
        len = oldText.length();
        blockRemoteRemove = true;
        kDebug() << "ERASING TEXT" << oldText << "with len" << len << "offset" << offset << "range" << range;
        kDebug() << offset << len << length();
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
    if( m_changeCount )
    {
        m_changeCount = 0;
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
    m_changeCount -= 1;
    Q_ASSERT(m_changeCount >= 0);
    if ( m_changeCount == 0 ) {
        emit canUndo(false);
    }
    m_undoCount += 1;
    emit canRedo(true);
}

void KDocumentTextBuffer::performingRedo()
{
    redo_lock = true;
    m_undoCount -= 1;
    Q_ASSERT(m_undoCount >= 0);
    if ( m_undoCount == 0 ) {
        emit canRedo(false);
    }
    m_changeCount += 1;
    emit canUndo(true);
}

unsigned int KDocumentTextBuffer::changeCount() const
{
    return m_changeCount;
}

unsigned int KDocumentTextBuffer::undoCount() const
{
    return m_undoCount;
}

unsigned int KDocumentTextBuffer::cursorToOffset_inf( const KTextEditor::Cursor &cursor )
{
    unsigned int offset = 0;
    int cursor_line = cursor.line();
    QList<QByteArray> lines = slice(0, length())->text().split('\n');
    kDebug() << lines;
    for( int i = 0; i < cursor_line; i++ ) {
        offset += codec()->toUnicode(lines.at(i)).length() + 1; // Add newline
    }
    offset += cursor.column();
    kDebug() << "cursor:" << cursor << "-> offset:" << offset;
    return offset;
}

KTextEditor::Cursor KDocumentTextBuffer::offsetToCursor_inf( unsigned int offset )
{
    int remaining = offset;
    QList<QByteArray> lines = slice(0, length())->text().split('\n');
    kDebug() << lines << offset;
    int i = 0;
    while ( remaining > codec()->toUnicode(lines.at(i)).length() ) {
        remaining -= codec()->toUnicode(lines.at(i)).length() + 1; // Subtract newline
        i++;
        if ( remaining == 0 ) {
            break;
        }
        if ( i == lines.size() ) {
            kWarning() << "oops, invalid offset?";
            Q_ASSERT(false);
            break;
        }
    }
    kDebug() << "offset" << offset << "-> Cursor(" << i << "," << remaining << ")";
    return KTextEditor::Cursor( i, remaining );
}

unsigned int KDocumentTextBuffer::cursorToOffset_kte( const KTextEditor::Cursor &cursor )
{
    unsigned int offset = 0;
    for( int i = 0; i < cursor.line(); i++ ) {
        kDebug() << "LINE" << i;
        offset += kDocument()->lineLength(i) + 1; // Add newline
    }
    offset += cursor.column();
    kDebug() << kDocument()->text() << cursor << offset;
    return offset;
}

KTextEditor::Cursor KDocumentTextBuffer::offsetToCursor_kte( unsigned int offset )
{
    int soff = 0;
    if( offset > 0 )
        soff = offset;
    int i;
    for( i = 0; soff > kDocument()->lineLength(i); i++ ) {
        soff -= kDocument()->lineLength(i) + 1; // Subtract newline
        if ( kDocument()->lineLength(i) == -1 ) {
            kWarning() << "oops, invalid offset?" << i << offset;
            break;
        }
    }
    return KTextEditor::Cursor( i, soff );
}

void KDocumentTextBuffer::textOpPerformed()
{
    if ( ! m_user ) {
        // cannot undo synchronization operations
        return;
    }
    if ( undo_lock ) {
        undo_lock = false;
    }
    else if( redo_lock ) {
        redo_lock = false;
    }
    else {
        m_changeCount += 1;
        if ( m_changeCount == 1 ) {
            emit canUndo(true);
        }
        if ( m_undoCount > 0 ) {
            m_undoCount = 0;
            emit canRedo(false);
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
    , m_user( 0 )
    , m_name( name )
{
    kDebug() << "new infTextDocument for url" << kDocument()->url();
    m_session->setParent( this );
    m_sessionProxy->setParent( this );
    connect( kDocument(), SIGNAL(viewCreated( KTextEditor::Document*, KTextEditor::View* )),
        this, SLOT(slotViewCreated( KTextEditor::Document*, KTextEditor::View* )) );
    foreach ( KTextEditor::View* view, kDocument()->views() ) {
        slotViewCreated(kDocument(), view);
    }
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
    kDebug() << "UNDO" << m_user;
    m_buffer->performingUndo();
    if( m_user ) {
        m_session->undo( *m_user, 1 );
    }
}

void InfTextDocument::redo()
{
    kDebug() << "REDO";
    m_buffer->performingRedo();
    if( m_user ) {
        m_session->redo( *m_user, 1 );
    }
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
    m_user = QInfinity::AdoptedUser::wrap(INF_ADOPTED_USER(user->gobject()));
    setLoadState( Document::JoiningComplete );
    setLoadState( Document::Complete );
    kDebug() << "Join successful, user" << user->name() << "now online" << m_user << INF_ADOPTED_USER(user->gobject());
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

