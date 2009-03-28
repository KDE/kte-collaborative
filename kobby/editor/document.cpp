#include "document.h"
#include "kobbysettings.h"

#include <libqinfinity/sessionproxy.h>
#include <libqinfinity/session.h>
#include <libqinfinity/textsession.h>
#include <libqinfinity/userrequest.h>
#include <libqinfinity/user.h>

#include <KDebug>

namespace Kobby
{

Document::Document( KTextEditor::Document &kDocument,
    QObject *parent )
    : QObject( parent )
    , m_kDocument( &kDocument )
{
    m_kDocument->setParent( this );
}

Document::~Document()
{
}

Document::Type Document::type() const
{
    return KDocument;
}

KTextEditor::Document *Document::kDocument() const
{
    return m_kDocument;
}

bool Document::save()
{
    return m_kDocument->documentSave();
}

QString Document::name()
{
    return m_kDocument->documentName();
}

InfTextDocument::InfTextDocument( KTextEditor::Document &kDocument,
    QPointer<QInfinity::SessionProxy> sessionProxy,
    QObject *parent )
    : Document( kDocument, parent )
    , m_sessionProxy( sessionProxy )
{
    kDocument.setReadWrite( false );
    QInfinity::Session *session = m_sessionProxy->session();
    if( session->status() == QInfinity::Session::Synchronizing )
    {
        kDebug() << "Waiting until synchronization completes.";
        connect( session, SIGNAL(synchronizationComplete()),
            this, SLOT(sessionRunning()) );
    }
    else if( session->status() == QInfinity::Session::Running )
        sessionRunning();
    else
        kDebug() << "Session closed.  Editing disabled.";
}

Document::Type InfTextDocument::type() const
{
    return Document::InfText;
}

void InfTextDocument::sessionRunning()
{
    QInfinity::Session *session = m_sessionProxy->session();
    if( session->type() != QInfinity::Session::Text )
    {
        kDebug() << "Cant join session of unknown type.  Editing disabled.";
        return;
    }
    QInfinity::UserRequest *req = QInfinity::TextSession::joinUser( m_sessionProxy,
        KobbySettings::nickName(),
        0 );
    connect( req, SIGNAL(finished( QPointer<QInfinity::User> )),
        this, SLOT(userJoined( QPointer<QInfinity::User> )) );
    connect( req, SIGNAL(failed( GError* )),
        this, SLOT(userJoinFailed( GError* )) );
}

void InfTextDocument::userJoined( QPointer<QInfinity::User> user )
{
    kDebug() << "User join successfull, enabling editing.";
    kDocument()->setReadWrite( true );
}

void InfTextDocument::userJoinFailed( GError *error )
{
    kDebug() << "User joining failed!";
}

}

