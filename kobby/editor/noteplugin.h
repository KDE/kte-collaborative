#ifndef KOBBY_NOTEPLUGIN_H
#define KOBBY_NOTEPLUGIN_H

#include <libqinfinity/noteplugin.h>

namespace Kobby
{

class DocumentBuilder;

/**
 * @brief Instantiates InfText sessions.
 */
class NotePlugin
    : public QInfinity::NotePlugin
{

    public:
        NotePlugin( DocumentBuilder &builder,
            QObject *parent = 0 );

        QInfinity::Session *createSession( QInfinity::CommunicationManager *commMgr,
            QInfinity::CommunicationJoinedGroup *syncGroup,
            QInfinity::XmlConnection *syncConnection );

    private:
        DocumentBuilder *m_builder;

};

}

#endif

