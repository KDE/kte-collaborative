#include <libinfinitymm/common/session.h>
#include <libinfinitymm/common/user.h>
#include <libinfinitymm/client/clientsessionproxy.h>
#include <libinfinitymm/client/clientuserrequest.h>
#include <libinfinitymm/adopted/adoptedstatevector.h>
#include <libinfinitymm/adopted/adoptedsession.h>
#include <libinftextmm/textchunk.h>
#include <libinftextmm/textbuffer.h>

#include "collabdocument.h"
#include "kobbysettings.h"

#include <libqinfinitymm/document.h>

#include <KTextEditor/Document>
#include <KTextEditor/Range>
#include <KTextEditor/Cursor>
#include <KTextEditor/View>

#include <KDebug>

#include <QBuffer>

#include <glib-object.h>

#include "collabdocument.moc"

namespace Kobby
{

void get_local_user( InfUser *user, gpointer user_data )
{
    (*static_cast<InfUser**>(user_data)) = user;
}

CollabDocument::CollabDocument( QInfinity::Session &session,
    KTextEditor::Document &document,
    QObject *parent )
    : QObject( parent )
    , m_session( &session )
    , m_textBuffer( 0 )
    , m_infSession( session.infSession() )
    , m_kDocument( &document )
    , m_sessionProxy( session.infSessionProxy() )
    , localUser( 0 )
{
    textStream.setCodec( "utf-8" );
    setupSessionActions();
}

CollabDocument::~CollabDocument()
{
    delete m_session;
}

KTextEditor::Document *CollabDocument::kDocument() const
{
    return m_kDocument;
}

QInfinity::Session &CollabDocument::session() const
{
    return *m_session;
}

void CollabDocument::slotLocalTextInserted( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    unsigned int pos;
    unsigned int size;

    if( localUser )
    {
        QString text = m_kDocument->text( range );
        pos = cursorToPos( range.start(), *m_kDocument );
        kDebug() << "Inserting " << text << "on " << range.start().line() 
            << ":" << range.start().column() << "to " << range.end().line() 
            << ":" << range.end().column() << " linear is " << pos;
        if( text[0] == '\n' )
            text = "\n";
        size = text.size();
        m_textBuffer->insertText( pos, text.toUtf8(), size, size, localUser );
    }
    else
        kDebug() << "No local user set.";
}

void CollabDocument::slotLocalEraseText( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    unsigned int pos = cursorToPos( range.start(), *document );
    debugRemove( document, range );
    KTextEditor::Cursor startCursor = range.start();
    KTextEditor::Cursor endCursor = range.end();
    unsigned int start = cursorToPos( startCursor );
    unsigned int end = cursorToPos( endCursor );
    unsigned int len = end - start;

    if( localUser )
    {
        m_textBuffer->eraseText( pos, len, *localUser );
    }
    else
        kDebug() << "No local user set.";
}

void CollabDocument::slotRemoteInsertText( unsigned int pos,
    Infinity::TextChunk textChunk,
    Infinity::User *user )

{
    if( user->gobj() == localUser->gobj() )
        return;

    unsigned int length;
    const char *text;
    text = (const char*)textChunk.getText( &length );
    
    QByteArray byteArr( text, length );
    QBuffer buffer( &byteArr );
    buffer.open( QBuffer::ReadOnly );
    textStream.setDevice( &buffer );


    m_kDocument->insertText( posToCursor( pos ), textStream.readAll() );
    textStream.setDevice( 0 );
}

void CollabDocument::slotRemoteEraseText( unsigned int pos,
    unsigned int len,
       Infinity::User *user )
{
    if( user->gobj() == localUser->gobj() )
        return;

    KTextEditor::Cursor start = posToCursor( pos );
    KTextEditor::Cursor end = posToCursor( pos + len );
    KTextEditor::Range range( start, end );

    m_kDocument->removeText( range );
}

void CollabDocument::slotLocalTextChanged( KTextEditor::Document *document,
    const KTextEditor::Range &oldRange,
    const KTextEditor::Range &newRange )
{
    kDebug() << "text changed";
}

void CollabDocument::slotSynchronizationComplete()
{
    kDebug() << "Sync complete.";
    joinUser();
    m_textBuffer = dynamic_cast<Infinity::TextBuffer*>(m_infSession->getBuffer());
    setupDocumentActions();
}

void CollabDocument::setupSessionActions()
{
    if( !m_infSession )
    {
        kDebug() << "Connecting session actions when no session is set!";
        return;
    }
    connect( &session(), SIGNAL(synchronizationComplete()),
        this, SLOT(slotSynchronizationComplete()) );
}

void CollabDocument::setupDocumentActions()
{
    m_textBuffer->signal_insertText().connect( sigc::mem_fun( this,
        &CollabDocument::slotRemoteInsertText ) );
    connect( m_kDocument, SIGNAL(textInserted( KTextEditor::Document*,
            const KTextEditor::Range& )),
        this, SLOT(slotLocalTextInserted( KTextEditor::Document*,
            const KTextEditor::Range& )) );
    m_textBuffer->signal_eraseText().connect( sigc::mem_fun( this,
        &CollabDocument::slotRemoteEraseText ) );
    connect( m_kDocument, SIGNAL(textRemoved( KTextEditor::Document*,
            const KTextEditor::Range& )),
        this, SLOT(slotLocalEraseText( KTextEditor::Document*,
            const KTextEditor::Range& )) );
    connect( m_kDocument, SIGNAL(textChanged( KTextEditor::Document*,
            const KTextEditor::Range&, const KTextEditor::Range& )),
        this, SLOT(slotLocalTextChanged( KTextEditor::Document*,
            const KTextEditor::Range&, const KTextEditor::Range& )) );
}

unsigned int CollabDocument::cursorToPos( const KTextEditor::Cursor &cursor, KTextEditor::Document &document )
{
    unsigned int pos = 0, i;
    for( i = 0; i < cursor.line(); i++ )
    {
        pos += document.lineLength( i );
        pos++;
    }
    pos += cursor.column();
    return pos;
}

void CollabDocument::userRequestFinished( Infinity::User *user )
{
    localUser = user;
}

void CollabDocument::joinUser()
{
    InfUser *infUser = 0;
    m_infSession->getUserTable()->forEachLocalUser( get_local_user, &infUser) ;
    if( infUser )
        localUser = Glib::wrap( infUser, true ).operator->();
    else
    {

        // 'Borrowed' from Gobby
        GParameter params[5] = {
            { "name", { 0 } },
            { "hue", { 0 } },
            { "vector", { 0 } },
            { "caret-position", { 0 } },
            { "status", { 0 } }
        };

        g_value_init(&params[0].value, G_TYPE_STRING);
        g_value_init(&params[1].value, G_TYPE_DOUBLE);
        g_value_init(&params[2].value, INF_ADOPTED_TYPE_STATE_VECTOR);
        g_value_init(&params[3].value, G_TYPE_UINT);
        g_value_init(&params[4].value, INF_TYPE_USER_STATUS);

        g_value_set_static_string(&params[0].value, KobbySettings::nickName().toAscii());
        g_value_set_double(&params[1].value, 0);
        g_value_take_boxed(
                &params[2].value,inf_adopted_state_vector_copy(
                        inf_adopted_algorithm_get_current(
                                inf_adopted_session_get_algorithm(
                                        INF_ADOPTED_SESSION(m_infSession->gobj())))));
        g_value_set_uint(&params[3].value, 0);
        g_value_set_enum(&params[4].value, INF_USER_ACTIVE);

        InfcUserRequest *infUserRequest;
        infUserRequest = infc_session_proxy_join_user( m_sessionProxy->gobj(), params, 5, 0 );
        userRequest = Glib::wrap( infUserRequest, true );
        if( userRequest )
        {
            kDebug() << "Successful join.";
            userRequest->signal_finished().connect( sigc::mem_fun( this, &CollabDocument::userRequestFinished ) );
        }
        else
            kDebug() << "Error joining user.";

    }
}

KTextEditor::Cursor CollabDocument::posToCursor( int pos ) const
{
    int line = 0;
    for ( ; line < kDocument()->lines()-1; ++line )
    {
        if (kDocument()->line(line).length() >= pos)
            break;

        pos -= kDocument()->line(line).length()+1;
    }
     
    return KTextEditor::Cursor(line, pos);
}

int CollabDocument::cursorToPos( KTextEditor::Cursor cursor ) const
{
    int line = cursor.line(), column = cursor.column();
    unsigned int pos = 0;
    for ( int curLine = 0; curLine < line; ++curLine )
    {
        pos += kDocument()->lineLength(curLine) + 1;
    }

    return pos + column;
}

void CollabDocument::debugInsert( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    kDebug() << "inserting " << document->text( range, true ) <<
        "on line " << range.start().line() << " column " << range.start().column()
        << " until " << range.end().line() << " column " << range.end().column();
}

void CollabDocument::debugRemove( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    kDebug() << "removed " <<
        "on line " << range.start().line() << " column " << range.start().column()
        << " until " << range.end().line() << " column " << range.end().column();
}

}

