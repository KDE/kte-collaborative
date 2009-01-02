#ifndef KOBBY_COLLABSESSION_H
#define KOBBY_COLLABSESSION_H

#include <QObject>

namespace Infinity
{
    class TextSession;
    class XmlConnection;
    class ConnectionManagerGroup;
}

namespace Kobby
{

class CollabSession
    : public QObject
{

    Q_OBJECT
    public:
        CollabSession( Infinity::TextSession &infTextSession,
            QObject *parent = 0 );

        Infinity::TextSession &textSession() const;

    Q_SIGNALS:
        void synchronizationBegin();
        void synchronizationComplete();

    private:
        void synchronizationBegin( Infinity::ConnectionManagerGroup *group, Infinity::XmlConnection *connection );
        void synchronizationComplete( Infinity::XmlConnection *connection );

        Infinity::TextSession *m_textSession;

};

}

#endif