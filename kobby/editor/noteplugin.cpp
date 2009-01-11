#include "noteplugin.h"

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
    
}

}

