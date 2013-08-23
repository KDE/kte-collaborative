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
#include "utils.h"

#include <libinftext/inf-text-undo-grouping.h>

#include <libqinfinity/undogrouping.h>
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
#include <KDialog>
#include <KLineEdit>
#include <KPushButton>

#include <QString>
#include <QTextCodec>
#include <QTextEncoder>
#include <QTime>
#include <QTemporaryFile>
#include <QAction>
#include <QFormLayout>
#include <QLabel>

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
    , m_session(0)
    , m_undoGrouping( QInfinity::UndoGrouping::wrap(inf_text_undo_grouping_new(), this) )
    , m_aboutToClose( false )
{
    kDebug() << "new text buffer for document" << kDocument;
    connect( kDocument, SIGNAL(textInserted(KTextEditor::Document*, const KTextEditor::Range&)),
        this, SLOT(localTextInserted(KTextEditor::Document*, const KTextEditor::Range&)) );
    connect( kDocument, SIGNAL(textRemoved(KTextEditor::Document*, const KTextEditor::Range&, const QString&)),
        this, SLOT(localTextRemoved(KTextEditor::Document*, const KTextEditor::Range&, const QString&)) );
    m_undoTimer.setInterval(300);
    m_undoTimer.setSingleShot(true);
    connect( &m_undoTimer, SIGNAL(timeout()),
        this, SLOT(nextUndoStep()) );
}

void KDocumentTextBuffer::nextUndoStep()
{
    kDebug() << "starting undo group";
    if ( m_undoGrouping->hasOpenGroup() ) {
        m_undoGrouping->endGroup();
    }
    m_undoGrouping->beginGroup();
}

KDocumentTextBuffer::~KDocumentTextBuffer()
{
}

void KDocumentTextBuffer::resetUndoRedo()
{
    emit canUndo(false);
    emit canRedo(false);
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
    if ( m_aboutToClose ) return;

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
            iface->insertTextSilent(startCursor, str);
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
        kDebug() << "done inserting text";
        emit remoteChangedText(KTextEditor::Range(startCursor, offsetToCursor_inf(offset+chunk.length())), user, false);
        checkConsistency();
    }
    else
        blockRemoteInsert = false;
}

void KDocumentTextBuffer::shutdown()
{
    m_aboutToClose = true;
    deleteLater();
}

void KDocumentTextBuffer::onEraseText( unsigned int offset,
    unsigned int length,
    QInfinity::User *user )
{
    if ( m_aboutToClose ) return;

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
            iface->removeTextSilent(KTextEditor::Range(startCursor, endCursor));
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
        kDebug() << "done removing text";
        emit remoteChangedText(range, user, true);
        checkConsistency();
    }
    else
        blockRemoteRemove = false;
}

void KDocumentTextBuffer::checkConsistency()
{
    QString bufferContents = codec()->toUnicode( slice(0, length())->text() );
    QString documentContents = kDocument()->text();
    if ( bufferContents != documentContents ) {
        KUrl url = kDocument()->url();
        kDocument()->setModified(false);
        kDocument()->setReadWrite(false);
        m_aboutToClose = true;
        QTemporaryFile f;
        f.setAutoRemove(false);
        f.open();
        f.close();
        kDocument()->saveAs(f.fileName());
        KDialog* dialog = new KDialog;
        dialog->setButtons(KDialog::Ok | KDialog::Cancel);
        QLabel* label = new QLabel(i18n("Sorry, an internal error occurred in the text synchronization component.<br>"
                                        "You can try to reload the document or disconnect."));
        label->setWordWrap(true);
        dialog->setMainWidget(label);
        dialog->button(KDialog::Ok)->setText(i18n("Reload document"));
        dialog->button(KDialog::Cancel)->setText(i18n("Disconnect"));
        DocumentReopenHelper* helper = new DocumentReopenHelper(url, kDocument());
        connect(dialog, SIGNAL(accepted()), helper, SLOT(reopen()));
        // We must not use exec() here, since that will create a nested event loop,
        // which might handle incoming network events. This can easily get very messy.
        dialog->show();
    }
}

void KDocumentTextBuffer::localTextInserted( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    if ( m_aboutToClose ) return;

    kDebug() << "local text inserted" << kDocument() << "(range" << range << ")" << m_user;
    emit localChangedText(range, user(), false);
    Q_UNUSED(document)

    textOpPerformed();
    unsigned int offset;
    if( m_user.isNull() ) {
        kDebug() << "Could not insert text: No local user set.";
        return;
    }
    offset = cursorToOffset_kte(range.start());
    QInfinity::TextChunk chunk(encoding());
    QString text = kDocument()->text(range);
#ifdef ENABLE_TAB_HACK
    if ( text.contains('\t') ) {
        text = text.replace('\t', "    ");
        kDocument()->blockSignals(true);
        kDocument()->replaceText(range, text);
        kDocument()->blockSignals(false);
    }
#endif
    Q_ASSERT(encoder());
    if ( text.isEmpty() ) {
        kDebug() << "Skipping empty insert.";
        return;
    }
    QByteArray encodedText = codec()->fromUnicode( text );
    if ( encodedText.size() == 0 ) {
        kDebug() << "Got empty encoded text from non empty string "
                    "Skipping insertion";
    }
    else {
        chunk.insertText( 0, encodedText, text.length(), m_user->id() );
        blockRemoteInsert = true;
        kDebug() << "inserting chunk of size" << chunk.length() << "into local buffer" << kDocument()->url();
        insertChunk( offset, chunk, m_user );
        checkConsistency();
    }
}

void KDocumentTextBuffer::localTextRemoved( KTextEditor::Document *document,
    const KTextEditor::Range &range, const QString& oldText )
{
    if ( m_aboutToClose ) return;

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
        checkConsistency();
    }
    else
        kDebug() << "Could not remove text: No local user set.";

}

void KDocumentTextBuffer::setUser( QPointer<QInfinity::User> user )
{
    m_user = user;
}

void KDocumentTextBuffer::updateUndoRedoActions()
{
    emit canUndo(dynamic_cast<QInfinity::AdoptedSession*>(m_session)->canUndo(
        *dynamic_cast<QInfinity::AdoptedUser*>(m_user.data()))
    );
    emit canRedo(dynamic_cast<QInfinity::AdoptedSession*>(m_session)->canRedo(
        *dynamic_cast<QInfinity::AdoptedUser*>(m_user.data()))
    );
}

void KDocumentTextBuffer::setSession(QInfinity::Session* session)
{
    m_session = session;
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
    else {
        kDebug() << "starting undo timer";
        m_undoTimer.start();
        updateUndoRedoActions();
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
    if( m_user ) {
        m_session->undo( *m_user, m_buffer->m_undoGrouping->undoSize() );
    }
    m_buffer->updateUndoRedoActions();
}

void InfTextDocument::redo()
{
    kDebug() << "REDO";
    if( m_user ) {
        m_session->redo( *m_user, m_buffer->m_undoGrouping->redoSize() );
    }
    m_buffer->updateUndoRedoActions();
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
    Q_ASSERT(user);
    m_buffer->m_undoGrouping->initialize(m_session, user);
    m_buffer->setUser( user );
    m_user = QInfinity::AdoptedUser::wrap(INF_ADOPTED_USER(user->gobject()));
    setLoadState( Document::JoiningComplete );
    setLoadState( Document::Complete );
    kDebug() << "Join successful, user" << user->name() << "now online" << m_user << INF_ADOPTED_USER(user->gobject());
}

void InfTextDocument::slotJoinFailed( GError *gerror )
{
    QString emsg = i18n( "Could not join session: " );
    if ( gerror ) {
        emsg.append(gerror->message);
    }
    else {
        emsg.append("Unknown error");
    }
    kDebug() << "Join failed: " << emsg;
    retryJoin(emsg);
}

void InfTextDocument::retryJoin(const QString& message)
{
    KDialog dialog;
    dialog.setButtons(KDialog::Ok | KDialog::Cancel);
    dialog.button(KDialog::Ok)->setText(i18n("Retry"));
    QWidget w;
    dialog.setMainWidget(&w);
    w.setLayout(new QVBoxLayout);
    w.layout()->addWidget(new QLabel(i18n("Failed to join editing session: %1", message)));
    w.layout()->addWidget(new QLabel(i18n("You can try joining again with a different user name:")));
    KLineEdit username;
    username.setClickMessage(i18n("Enter your user name..."));
    w.layout()->addWidget(&username);
    username.setFocus();
    if ( dialog.exec() ) {
        joinSession(username.text());
    }
    else {
        throwFatalError(QString());
    }
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
    kDebug() << "SET UNDO:" << enable;
    QAction *act;
    foreach( act, undoActions )
    {
        act->setEnabled( enable );
    }
}

void InfTextDocument::slotCanRedo( bool enable )
{
    kDebug() << "SET REDO:" << enable;
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
        {
            ReadWriteTransaction t(kDocument());
            kDocument()->clear();
        }
        kDebug() << "document contents at sync begin:" << kDocument()->text();
        setLoadState( Document::Synchronizing );
        connect( m_session, SIGNAL(synchronizationComplete()),
            this, SLOT(slotSynchronized()) );
        connect( m_session, SIGNAL(synchronizationFailed( GError* )),
            this, SLOT(slotSynchronizationFailed( GError* )) );
    }
}

void InfTextDocument::joinSession(const QString& forceUserName)
{
    if( m_session->status() == QInfinity::Session::Running )
    {
        // We dont want this being called again
        disconnect( this, SLOT(joinSession()) );
        
        setLoadState( Document::Joining );
        QString userName;
        if ( ! forceUserName.isEmpty() ) {
            userName = forceUserName;
        }
        else if ( ! kDocument()->url().userName().isEmpty() ) {
            userName = kDocument()->url().userName();
        }
        else {
            userName = "UnnamedUser_" + QString::number(qHash(QString::number(QTime::currentTime().second())
                                                        + QString::number(QTime::currentTime().msec())));
        }
        kDebug() << "requesting join of user" << userName << ColorHelper::colorForUsername(userName).hue();
        QInfinity::UserRequest *req = QInfinity::TextSession::joinUser( m_sessionProxy,
            *m_session,
            userName,
            ColorHelper::colorForUsername(userName).hue() / 360.0 );
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

