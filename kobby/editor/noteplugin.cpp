#include "noteplugin.h"

namespace Kobby
{

NotePlugin::NotePlugin()
    : Infinity::ClientNotePlugin( "InfText" )
{
}

InfSession *createSession( InfIo *io,
    InfConnectionManager *manager,
    InfConnectionManagerGroup *sync_group,
    InfXmlConnection *sync_connection )
{
    
}

}

