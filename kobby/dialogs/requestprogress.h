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

#ifndef KOBBY_REQUESTPROGRESS_H
#define KOBBY_REQUESTPROGRESS_H

#include <KDialog>

#include <QWidget>

#include <glibmm/refptr.h>

#include <glib/gerror.h>

namespace Ui
{
    class RequestProgressWidget;
}

namespace Infinity
{
    class ClientNodeRequest;
}

namespace Kobby
{

class RequestProgressWidget
    : public QWidget
{

    Q_OBJECT

    public:
        RequestProgressWidget( const Glib::RefPtr<Infinity::ClientNodeRequest> &request,
            const QString &text = "Running request.",
            QWidget *parent = 0);

    Q_SIGNALS:
        void requestFinished();
        void requestFailed();

    private:
        void setupUi();
        void setupActions();
        void requestFinishedCb( Infinity::ClientBrowserIter iter );
        void requestFailedCb( const GError* );

        Ui::RequestProgressWidget *ui;
        Glib::RefPtr<Infinity::ClientNodeRequest> *request;
        QString text;

};

class RequestProgressDialog
    : public KDialog
{

    public:
        RequestProgressDialog( const Glib::RefPtr<Infinity::ClientNodeRequest> &request,
            const QString &text = "Running request.",
            QWidget *parent = 0);

    private:
        RequestProgressWidget *mainWidget;

};

}

#endif
