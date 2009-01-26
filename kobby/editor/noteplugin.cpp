#include "noteplugin.h"

#include <libinfinity/common/inf-user-table.h>
#include <libinftext/inf-text-session.h>
#include <libinftext/inf-text-default-buffer.h>

namespace Kobby
{

NotePlugin::NotePlugin()
    : Infinity::ClientNotePlugin( "InfText" )
{
}

NotePlugin::~NotePlugin()
{
}

InfSession *NotePlugin::createSession( InfIo *io,
    InfConnectionManager *manager,
    InfConnectionManagerGroup *sync_group,
    InfXmlConnection *sync_connection )
{
    InfUserTable *userTable = inf_user_table_new();
    InfTextDefaultBuffer *buffer = inf_text_default_buffer_new( "utf-8" );
    InfTextSession *textSession = inf_text_session_new_with_user_table( manager,
        INF_TEXT_BUFFER(buffer),
        io,
        userTable,
        sync_group,
        sync_connection );
    return INF_SESSION(textSession);
}

}

