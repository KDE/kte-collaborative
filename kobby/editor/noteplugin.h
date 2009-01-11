#ifndef KOBBY_NOTEPLUGIN_H
#define KOBBY_NOTEPLUGIN_H

// Make sure to include this header BEFORE any Qt headers!
#include <libinfinitymm/client/clientnoteplugin.h>

namespace Kobby
{

/**
 * @brief Instantiates InfText sessions.
 */
class NotePlugin
    : public Infinity::ClientNotePlugin
{

    public:
        NotePlugin();
        ~NotePlugin();

    protected:
        InfSession *createSession( InfIo *io,
            InfConnectionManager *manager,
            InfConnectionManagerGroup *sync_group,
            InfXmlConnection *sync_connection );

};

}

#endif

