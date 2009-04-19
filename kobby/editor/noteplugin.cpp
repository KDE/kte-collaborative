#include "noteplugin.h"
#include "document.h"
#include "documentbuilder.h"

#include <KTextEditor/Document>

#include <libqinfinity/textsession.h>

#include <libinfinity/common/inf-user-table.h>
#include <libinftext/inf-text-session.h>
#include <libinftext/inf-text-default-buffer.h>

namespace Kobby
{

NotePlugin::NotePlugin( DocumentBuilder &builder,
    QObject *parent )
    : QInfinity::NotePlugin( "InfText", parent )
    , m_builder( &builder )
{
}

QInfinity::Session *NotePlugin::createSession( QInfinity::CommunicationManager *commMgr,
    QInfinity::CommunicationJoinedGroup *syncGroup,
    QInfinity::XmlConnection *syncConnection )
{
    KDocumentTextBuffer *document = m_builder->createKDocumentTextBuffer( "UTF-8" );
    KDocumentTextBuffer *buffer = new KDocumentTextBuffer( *document->kDocument(),
        "UTF-8", this );
    QInfinity::TextSession *session = new QInfinity::TextSession( *commMgr,
        *buffer,
        *syncGroup,
        *syncConnection );
    return session;
}

}

