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
#include "noteplugin.h"

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

#include <KTextEditor/ConfigInterface>

namespace Kobby
{

Document::Document( KTextEditor::Document* kDocument )
    : m_kDocument( kDocument )
    , m_loadState( Document::Unloaded )
    , m_dirty( false )
{
    m_kDocument->setParent( 0 );
    KTextEditor::ConfigInterface* iface = qobject_cast<KTextEditor::ConfigInterface*>(m_kDocument);
    iface->setConfigValue("replace-tabs", false);
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
    if( state != loadState() )
    {
        m_loadState = state;
        emit( loadStateChanged( this, state ) );
        if( state == Document::Complete ) {
            emit( loadingComplete( this ) );
        }
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
    Kobby::NotePlugin* plugin,
    QObject *parent )
    : QInfinity::AbstractTextBuffer( encoding, parent )
    , blockRemoteInsert( false )
    , blockRemoteRemove( false )
    , m_kDocument( kDocument )
    , m_session(0)
    , m_undoGrouping( QInfinity::UndoGrouping::wrap(inf_text_undo_grouping_new(), this) )
    , m_aboutToClose( false )
{
    plugin->registerTextBuffer(kDocument->url().path(), this);
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
        kDebug() << "REMOTE INSERT TEXT offset" << offset << kDocument()
                 << "(" << chunk.length() << " chars )" << kDocument()->url();
        KTextEditor::Cursor startCursor = offsetToCursor_kte( offset );
        QString str = codec()->toUnicode( chunk.text() );
        ReadWriteTransaction transaction(kDocument());
        kDocument()->blockSignals(true);
        kDocument()->insertText( startCursor, str );
        kDocument()->blockSignals(false);
        Q_ASSERT(!qobject_cast<KTextEditor::ConfigInterface*>(kDocument())->configValue("replace-tabs").toBool());
        emit remoteChangedText(KTextEditor::Range(startCursor, offsetToCursor_kte(offset+chunk.length())), user, false);
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
        KTextEditor::Cursor startCursor = offsetRelativeTo_kte(KTextEditor::Cursor(0, 0), offset);
        KTextEditor::Cursor endCursor = offsetRelativeTo_kte(startCursor, length);
        KTextEditor::Range range = KTextEditor::Range(startCursor, endCursor);
        ReadWriteTransaction transaction(kDocument());
#ifdef KTEXTEDITOR_HAS_BUFFER_IFACE
        // see onInsertText
        if ( KTextEditor::BufferInterface* iface = qobject_cast<KTextEditor::BufferInterface*>(kDocument()) ) {
            iface->removeTextSilent(KTextEditor::Range(startCursor, endCursor));
        }
#else
        if ( false ) { }
#endif
        else {
            kDocument()->blockSignals(true);
            kDocument()->removeText( range );
            kDocument()->blockSignals(false);
        }
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

    emit localChangedText(range, user(), false);
    Q_UNUSED(document)

    textOpPerformed();
    if( m_user.isNull() ) {
        kDebug() << "Could not insert text: No local user set.";
        return;
    }
    unsigned int offset = cursorToOffset_kte(range.start());
    kDebug() << "local text inserted" << kDocument() << "( range" << range << ")" << m_user << "offset:" << offset;
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
        chunk.insertText( 0, encodedText, countUnicodeCharacters(text), m_user->id() );
        blockRemoteInsert = true;
        kDebug() << "inserting chunk of size" << chunk.length() << "into local buffer" << kDocument()->url();
        insertChunk( offset, chunk, m_user );
        kDebug() << "done inserting chunk";
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

    textOpPerformed();
    if( !m_user.isNull() )
    {
        unsigned int offset = cursorToOffset_kte( range.start() );
        unsigned int len = countUnicodeCharacters(oldText);
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
    qDebug() << "SET USER:" << m_user;
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

// Gives the offset in str which corresponds to @p codePoints unicode code points
// codePoints will be 0 when all characters could be converted.
int surrogatesForCodePoints(const QString& str, unsigned int& codePoints) {
    int offset = 0;
    const int available = str.length();
    Q_ASSERT( str.size() == 0 || ! str[0].isLowSurrogate() ); // first two bytes must not be low surrogate
    for ( ; codePoints > 0; codePoints -- ) {
        if ( offset >= available ) {
            break;
        }
        if ( str[offset].isHighSurrogate() ) {
            // high surrogate
            offset += 2;
        }
        else {
            // normal character
            offset += 1;
        }
    }
    return offset;
}

int countUnicodeCharacters(const QString& str) {
    int characters = 0;
    int len = str.length();
    int pos = 0;
    while ( pos < len ) {
        characters += 1;
        if ( str[pos].isHighSurrogate() ) {
            pos += 2;
        }
        else {
            pos += 1;
        }
    }
    return characters;
}

KTextEditor::Cursor KDocumentTextBuffer::offsetRelativeTo_kte(const KTextEditor::Cursor& cursor, unsigned int offset)
{
    int lineno = cursor.line();
    const QString& firstLine = kDocument()->line(lineno).mid(cursor.column());
    unsigned int remaining = offset;
    int surrogates = surrogatesForCodePoints(firstLine, remaining);
    while ( remaining > 0 ) {
        remaining -= 1; // for the newline character
        lineno += 1;
        if ( remaining == 0 ) {
            surrogates = 0;
            break;
        }
        const QString& line = kDocument()->line(lineno);
        Q_ASSERT( lineno < kDocument()->lines() );
        surrogates = surrogatesForCodePoints(line, remaining);
    }
    return KTextEditor::Cursor(lineno, lineno == cursor.line() ? cursor.column() + surrogates : surrogates);
}

KTextEditor::Cursor KDocumentTextBuffer::offsetToCursor_kte( unsigned int offset )
{
    return offsetRelativeTo_kte(KTextEditor::Cursor(0, 0), offset);
}

unsigned int KDocumentTextBuffer::cursorToOffset_kte( const KTextEditor::Cursor &cursor )
{
    unsigned int offset = 0;
    for( int i = 0; i < cursor.line(); i++ ) {
        offset += countUnicodeCharacters(kDocument()->line(i)) + 1; // Add newline
    }
    offset += countUnicodeCharacters(kDocument()->line(cursor.line()).left(cursor.column()));
    return offset;
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

void KDocumentTextBuffer::checkLineEndings()
{
    QString bufferContents = kDocument()->text();
    if ( bufferContents.contains("\r\n") || bufferContents.contains("\r") ) {
        KDialog* dlg = new KDialog(kDocument()->activeView());
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setButtons(KDialog::Ok | KDialog::Cancel);
        dlg->button(KDialog::Ok)->setText(i18n("Continue"));
        QLabel* l = new QLabel(i18n("The document you opened contains non-standard line endings. "
                                    "Do you want to convert them to the standard \"\\n\" format?<br><br>"
                                    "<i>Note: This change will be synchronized to the server.</i>"), dlg);
        l->setWordWrap(true);
        dlg->setMainWidget(l);
        connect(dlg, SIGNAL(okClicked()), this, SLOT(replaceLineEndings()));
        dlg->show();
    }
}

void KDocumentTextBuffer::replaceLineEndings()
{
    const QStringList lines = kDocument()->textLines( KTextEditor::Range(
        KTextEditor::Cursor::start(),
        KTextEditor::Cursor(kDocument()->lines(), kDocument()->lineLength(kDocument()->lines() - 1) )) );
    for ( int i = lines.count() - 1; i >= 0; i-- ) {
        QString line = lines[i];
        int offset = 0;
        while ( ( offset = line.lastIndexOf('\r') ) != -1 ) {
            int replaceLen = 1;
            if ( offset - 1 < line.length() ) {
                // check if this \r is part of a \r\n
                if ( line[offset+1] == '\n' ) {
                    replaceLen = 2;
                }
            }
            KTextEditor::Cursor replaceStart(i, offset);
            KTextEditor::Cursor replaceEnd(i, offset+replaceLen);
            KTextEditor::Range replaceRange = KTextEditor::Range(replaceStart, replaceEnd);
            kDocument()->replaceText(replaceRange, "\n");
            line.replace(offset, replaceLen, '\n');
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
    kDocument()->setModified(false);
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
    m_buffer->checkLineEndings();
    kDebug() << "Join successful, user" << user->name() << "now online" << m_user << INF_ADOPTED_USER(user->gobject());
    kDebug() << "in document" << kDocument()->url();
}

void InfTextDocument::slotJoinFailed( const GError *gerror )
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
    KDialog* dialog = new KDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setButtons(KDialog::Ok | KDialog::Cancel);
    dialog->button(KDialog::Ok)->setText(i18n("Retry"));
    QWidget* w = new QWidget();
    dialog->setMainWidget(w);
    w->setLayout(new QVBoxLayout);
    w->layout()->addWidget(new QLabel(i18n("Failed to join editing session: %1", message)));
    w->layout()->addWidget(new QLabel(i18n("You can try joining again with a different user name:")));
    KLineEdit* username = new KLineEdit;
    username->setClickMessage(i18n("Enter your user name..."));
    w->layout()->addWidget(username);
    username->setFocus();
    connect(dialog, SIGNAL(okClicked()), SLOT(newUserNameEntered()));
    connect(dialog, SIGNAL(cancelClicked()), SLOT(joinAborted()));
    dialog->show();
}

void InfTextDocument::joinAborted()
{
    throwFatalError(i18n("No acceptable user name was given."));
}

void InfTextDocument::newUserNameEntered()
{
    KDialog* dlg = qobject_cast<KDialog*>(QObject::sender());
    KLineEdit* username = dlg->findChild<KLineEdit*>();
    joinSession(username->text());
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
            userName = getUserName();
        }
        kDebug() << "requesting join of user" << userName << ColorHelper::colorForUsername(userName).hue();
        QInfinity::UserRequest *req = QInfinity::TextSession::joinUser( m_sessionProxy,
            *m_session,
            userName,
            ColorHelper::colorForUsername(userName).hue() / 360.0 );
        connect( req, SIGNAL(finished(QPointer<QInfinity::User>)),
            this, SLOT(slotJoinFinished(QPointer<QInfinity::User>)) );
        connect( req, SIGNAL(failed(const GError*)),
            this, SLOT(slotJoinFailed(const GError*)) );
    }
    else
        connect( m_session, SIGNAL(statusChanged()),
            this, SLOT(joinSession()), Qt::UniqueConnection );
}

}

