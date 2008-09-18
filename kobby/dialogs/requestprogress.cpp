#include <libinfinitymm/client/clientnoderequest.h>

#include "requestprogress.h"

#include "kobby/ui_requestprogresswidget.h"

namespace Kobby
{

RequestProgressWidget::RequestProgressWidget( const Glib::RefPtr<Infinity::ClientNodeRequest> &crequest,
    const QString &ctext,
    QWidget *parent )
    : QWidget( parent )
    , ui( new Ui::RequestProgressWidget )
    , request( new Glib::RefPtr<Infinity::ClientNodeRequest> )
    , text( ctext )
{
    *request = crequest;
    setupUi();
    setupActions();
}

void RequestProgressWidget::setupUi()
{
    ui->setupUi( this );
    ui->statusLabel->setText( text );
}

void RequestProgressWidget::setupActions()
{
    (*request)->signal_finished().connect( sigc::mem_fun(
        this, &RequestProgressWidget::requestFinishedCb ) );
    (*request)->signal_failed().connect( sigc::mem_fun(
        this, &RequestProgressWidget::requestFailedCb ) );
}

void RequestProgressWidget::requestFinishedCb( Infinity::ClientBrowserIter iter )
{
    ui->statusLabel->setText( "Finished." );
    emit( requestFinished() );
}

void RequestProgressWidget::requestFailedCb( const GError *err )
{
    ui->statusLabel->setText( "Failed!" );
    emit( requestFailed() );
}

RequestProgressDialog::RequestProgressDialog( const Glib::RefPtr<Infinity::ClientNodeRequest> &crequest,
    const QString &ctext,
    QWidget *parent )
    : KDialog( parent )
    , mainWidget( new RequestProgressWidget( crequest, ctext, this ) )
{
    setMainWidget( mainWidget );
}

}
