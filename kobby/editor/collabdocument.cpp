#include <libinfinitymm/common/session.h>
#include <libinfinitymm/common/user.h>
#include <libinfinitymm/client/clientsessionproxy.h>
#include <libinfinitymm/client/clientuserrequest.h>
#include <libinfinitymm/adopted/adoptedstatevector.h>
#include <libinfinitymm/adopted/adoptedsession.h>
#include <libinftextmm/textchunk.h>
#include <libinftextmm/textbuffer.h>

#include "collabdocument.h"

#include <libqinfinitymm/document.h>

#include <KTextEditor/Document>
#include <KTextEditor/Range>
#include <KTextEditor/Cursor>

#include <KDebug>

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
    , local_pass( 0 )
{
    setupSessionActions();
}

CollabDocument::~CollabDocument()
{
    m_infSession->close();
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
    unsigned int pos = cursorToPos( range.start(), *document );

    QString text = document->text( range, true );
    if( localUser )
    {
        local_pass = 1;
        m_textBuffer->insertText( pos, text.toUtf8(), text.size(), text.size(), localUser );
    }
    else
        kDebug() << "No local user set.";
}

void CollabDocument::slotRemoteInsertText( unsigned int pos,
    Infinity::TextChunk textChunk,
    Infinity::User *user )

{
    Q_UNUSED(user)

    if( local_pass )
    {
        local_pass = 0;
        return;
    }

    int len = textChunk.getLength();
    KTextEditor::Cursor cursor = posToCursor( pos );
    QString text = QString::fromUtf8( (const char*)textChunk.getText( (gsize*)&len ), len );

    m_kDocument->insertText( cursor, text, (int)len );
    local_pass = 0;
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
    m_infSession->property_status().signal_changed().connect( sigc::mem_fun( this,
        &CollabDocument::sessionStatusChanged ) );
}

void CollabDocument::setupDocumentActions()
{
    m_textBuffer->signal_insertText().connect( sigc::mem_fun( this,
        &CollabDocument::slotRemoteInsertText ) );
    connect( m_kDocument, SIGNAL(textInserted( KTextEditor::Document*,
            const KTextEditor::Range& )),
        this, SLOT(slotLocalTextInserted( KTextEditor::Document*,
            const KTextEditor::Range& )) );
         
}

unsigned int CollabDocument::cursorToPos( const KTextEditor::Cursor &cursor, KTextEditor::Document &document )
{
    int pos = 0, i;
    for( i = 0; i < cursor.line(); i++ )
    {
        pos += document.lineLength( i );
    }
    pos += cursor.column();
    unsigned int upos = pos;
    return upos;
}

void CollabDocument::sessionStatusChanged()
{
    if( !m_infSession )
        return;
    if( m_infSession->getStatus() == Infinity::SESSION_RUNNING )
    {
    }
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

        g_value_set_static_string(&params[0].value, "gregha");
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
        pos += kDocument()->line(curLine).length() + 1;
    }

    return pos + column;
}

}

