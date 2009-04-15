#ifndef KOBBY_NOTEPLUGIN_H
#define KOBBY_NOTEPLUGIN_H

#include <libqinfinity/noteplugin.h>

namespace Kobby
{

/**
 * @brief Instantiates InfText sessions.
 */
class NotePlugin
    : public QInfinity::NotePlugin
{

    public:
        NotePlugin( QObject *parent = 0 );

        QInfinity::Session *createSession( QInfinity::CommunicationManager *commMgr,
            QInfinity::CommunicationJoinedGroup *syncGroup,
            QInfinity::XmlConnection *syncConnection );

};

}

#endif

