#ifndef KOBBY_NOTEPLUGIN_H
#define KOBBY_NOTEPLUGIN_H

typedef InfIo struct _InfIo;
typedef InfConnectionManager struct _InfConnectionManager;

namespace Kobby
{

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

