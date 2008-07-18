// This file may be licensed under the terms of of the
// GNU General Public License Version 2 (the "GPL").
// 
// Software distributed under the License is distributed 
// on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
// express or implied. See the GPL for the specific language 
// governing rights and limitations.
//
// You should have received a copy of the GPL along with this 
// program. If not, go to http://www.gnu.org/licenses/gpl.html
// or write to the Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

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
