#ifndef KOBBY_INFINOTEMANAGER_H
#define KOBBY_INFINOTEMANAGER_H

#include <QObject>
#include <QList>

namespace Infinity
{
    class XmppConnection;
};

typedef int gnutls_certificate_credentials_t;
typedef struct _Gsasl Gsasl;

namespace Kobby
{

class InfinoteManager : public QObject
{
    
    Q_OBJECT
    
    public:
        InfinoteManager();
        ~InfinoteManager();
        
        Infinity::XmppConnection &newXmppConnection( const char *host, 
            unsigned int port,
            const char *jid,
            gnutls_certificate_credentials_t cred,
            Gsasl *sasl_context
        );
    
    private:
        QList<Infinity::XmppConnection*> connections;
    
}; // class InfinoteManager

} // namespace Kobby

#endif
