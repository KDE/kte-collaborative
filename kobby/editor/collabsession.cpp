#include <libinfinitymm/common/xmlconnection.h>
#include <libinfinitymm/common/connectionmanagergroup.h>
#include <libinftextmm/textsession.h>

#include "collabsession.h"

namespace Kobby
{

CollabSession::CollabSession( Infinity::TextSession &infTextSession,
    QObject *parent )
    : QObject( parent )
    , m_textSession( &infTextSession )
{
}

Infinity::TextSession &CollabSession::textSession() const
{
    return *m_textSession;
}

}
