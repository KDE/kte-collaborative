#include <libinfinitymm/common/session.h>
#include <libinfinitymm/common/user.h>
#include <libinfinitymm/client/clientsessionproxy.h>
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

CollabDocument::CollabDocument( Glib::RefPtr<Infinity::ClientSessionProxy> &sessionProxy,
    KTextEditor::Document &document,
    QObject *parent )
    : QObject( parent )
    , m_textBuffer( 0 )
    , m_infSession( sessionProxy->getSession() )
    , m_kDocument( &document )
    , m_sessionProxy( new Glib::RefPtr<Infinity::ClientSessionProxy>() )
{
    *m_sessionProxy = sessionProxy;
    setupSessionActions();
}

CollabDocument::~CollabDocument()
{
    if( m_sessionProxy )
        delete m_sessionProxy;
    m_infSession->close();
}

KTextEditor::Document *CollabDocument::kDocument() const
{
    return m_kDocument;
}

void CollabDocument::slotLocalTextInserted( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
    unsigned int pos = cursorToPos( range.start(), *document );
    QString text = document->text( range, true );
    m_textBuffer->insertText( pos, (void*)text.toAscii().constData(), text.length(), text.length(), 0 );
    kDebug() << "Text inserted.";
}

void CollabDocument::slotInsertText( unsigned int pos,
    Infinity::TextChunk textChunk,
    Infinity::User *user )
{
    kDebug() << "Insert text";
}

void CollabDocument::setupSessionActions()
{
    if( !m_infSession )
    {
        kDebug() << "Connecting session actions when no session is set!";
        return;
    }
    m_infSession->signal_synchronizationComplete().connect( sigc::mem_fun( this,
        &CollabDocument::sessionSynchronizationComplete ) );
    m_infSession->property_status().signal_changed().connect( sigc::mem_fun( this,
        &CollabDocument::sessionStatusChanged ) );
}

void CollabDocument::setupDocumentActions()
{
    m_textBuffer->signal_insertText().connect( sigc::mem_fun( this,
        &CollabDocument::slotInsertText ) );
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

void CollabDocument::sessionSynchronizationComplete( Infinity::XmlConnection *connection )
{
    m_textBuffer = dynamic_cast<Infinity::TextBuffer*>(m_infSession->getBuffer());
    setupDocumentActions();
}

void CollabDocument::sessionStatusChanged()
{
    if( !m_infSession )
        return;
    if( m_infSession->getStatus() == Infinity::SESSION_RUNNING )
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

        g_value_set_static_string(&params[0].value, "greghaynes");
        g_value_set_double(&params[1].value, 0);
        g_value_take_boxed(
                &params[2].value,inf_adopted_state_vector_copy(
                        inf_adopted_algorithm_get_current(
                                inf_adopted_session_get_algorithm(
                                        INF_ADOPTED_SESSION(m_infSession->gobj())))));

        infc_session_proxy_join_user( (*m_sessionProxy)->gobj(), params, 5, 0 );
    }
}

}

