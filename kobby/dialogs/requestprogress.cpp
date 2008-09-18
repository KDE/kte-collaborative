#include <libinfinitymm/client/clientnoderequest.h>

#include "requestprogress.h"

#include "kobby/ui_requestprogresswidget.h"

#include <KDebug>

namespace Kobby
{

RequestProgressDialog::RequestProgressDialog( const QString &ctext,
    QWidget *parent )
    : KProgressDialog( parent, ctext, ctext )
    , completed_requests( 0 )
{
}

void RequestProgressDialog::addRequest( Glib::RefPtr<Infinity::ClientNodeRequest> request )
{
    Glib::RefPtr<Infinity::ClientNodeRequest> *localReq = new Glib::RefPtr<Infinity::ClientNodeRequest>;
    *localReq = request;
    (*localReq)->signal_finished().connect( sigc::mem_fun(
        this, &RequestProgressDialog::requestFinishedCb ) );
    (*localReq)->signal_failed().connect( sigc::mem_fun(
        this, &RequestProgressDialog::requestFailedCb ) );
    requests += localReq;

    progressBar()->setMaximum( requests.size() );
}

void RequestProgressDialog::requestFinishedCb( Infinity::ClientBrowserIter iter )
{
    kDebug() << "Request finished.";
    completed_requests++;
    progressBar()->setValue( completed_requests );
}

void RequestProgressDialog::requestFailedCb( const GError *err )
{
    setLabelText( "Request failed!" );
    completed_requests++;
    progressBar()->setValue( completed_requests );
}

}
