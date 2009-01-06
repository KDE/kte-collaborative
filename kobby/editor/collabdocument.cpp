#include <libinfinitymm/common/session.h>
#include <libinfinitymm/common/user.h>
#include <libinfinitymm/client/clientsessionproxy.h>
#include <libinftextmm/textchunk.h>

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
    unsigned int pos = 0, i;
    for( i = 0; i < cursor.line(); i++ )
    {
        pos += document.lineLength( i );
    }
    pos += cursor.column();
    return pos;
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
        GParameter *userParam = g_new( GParameter, 1 );
        userParam[0].value.g_type = 0;
        userParam[0].name = "name";
        g_value_init( &userParam[0].value, G_TYPE_STRING );
        g_value_set_string( &userParam[0].value, "greghaynes" );
        infc_session_proxy_join_user( (*m_sessionProxy)->gobj(), userParam, 1, 0 );
        g_free( userParam );
    }
}

}

