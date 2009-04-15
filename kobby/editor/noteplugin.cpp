#include "noteplugin.h"

#include <libqinfinity/abstracttextbuffer.h>

#include <libinfinity/common/inf-user-table.h>
#include <libinftext/inf-text-session.h>
#include <libinftext/inf-text-default-buffer.h>

namespace Kobby
{

NotePlugin::NotePlugin( QObject *parent )
    : QInfinity::NotePlugin( "InfText", parent )
{
}

QInfinity::Session *NotePlugin::createSession( QInfinity::CommunicationManager *commMgr,
    QInfinity::CommunicationJoinedGroup *syncGroup,
    QInfinity::XmlConnection *syncConnection )
{
    QInfinity::AbstractTextBuffer *buffer = new QInfinity::AbstractTextBuffer( "UTF-8", this );
    QInfinity::TextSession *session = new QInfinity::TextSession( *commMgr,
        *buffer,
        *syncGroup,
        *syncConnection );
    return TextSession;
}

}

