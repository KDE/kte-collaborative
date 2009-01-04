#include <libinfinitymm/common/session.h>
#include <libinfinitymm/common/user.h>
#include <libinfinitymm/client/clientsessionproxy.h>
#include <libinftextmm/textchunk.h>

#include "collabdocument.h"

#include <libqinfinitymm/document.h>

#include <KTextEditor/Document>
#include <KTextEditor/Range>
#include <KDebug>

namespace Kobby
{

CollabDocument::CollabDocument( Infinity::Session &session,
    KTextEditor::Document &document,
    QObject *parent )
    : QObject( parent )
    , m_infDocument( 0 )
    , m_infSession( &session )
    , m_kDocument( &document )
    , m_sessionProxy( 0 )
{
}

CollabDocument::CollabDocument( Glib::RefPtr<Infinity::ClientSessionProxy> &sessionProxy,
    KTextEditor::Document &document,
    QObject *parent )
    : QObject( parent )
    , m_infDocument( 0 )
    , m_infSession( 0 )
    , m_kDocument( &document )
    , m_sessionProxy( new Glib::RefPtr<Infinity::ClientSessionProxy>() )
{
    *m_sessionProxy = sessionProxy;
}

CollabDocument::~CollabDocument()
{
    delete m_infDocument;
}

KTextEditor::Document *CollabDocument::kDocument() const
{
    return m_kDocument;
}

void CollabDocument::slotLocalTextInserted( KTextEditor::Document *document,
    const KTextEditor::Range &range )
{
}

void CollabDocument::slotInsertText( unsigned int pos,
    Infinity::TextChunk textChunk,
    Infinity::User *user )
{
}

void CollabDocument::setupSessionProxyActions()
{
}

void CollabDocument::setupDocumentActions()
{
    connect( m_infDocument, SIGNAL(textInserted( unsigned int,
            Infinity::TextChunk, Infinity::User )),
        this, SLOT(slotInsertText(unsigned int,
            Infinity::TextChunk, Infinity::User )) );
    connect( m_kDocument, SIGNAL(textInserted( KTextEditor::Document*,
            KTextEditor::Range& )),
        this, SLOT(slotLocalTextInserted( KTextEditor::Document*,
            KTextEditor::Range& )) );
         
}

}

