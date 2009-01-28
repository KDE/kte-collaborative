#include "documentmanager.h"
#include "collabdocument.h"

#include <libqinfinitymm/browsermodel.h>
#include <libqinfinitymm/session.h>

#include <KTextEditor/Editor>
#include <KTextEditor/Document>
#include <KDebug>

namespace Kobby
{

DocumentManager::DocumentManager( KTextEditor::Editor &editor,
    QInfinity::BrowserModel &browserModel )
    : editor( &editor )
    , m_browserModel( &browserModel )
{
    setupSignals();
}

DocumentManager::~DocumentManager()
{
}

void DocumentManager::slotSessionSubscribed( QInfinity::BrowserNoteItem &note,
    QInfinity::Session session )
{
    insertSession( session );    
}

void DocumentManager::slotSessionSynchronized()
{
    QInfinity::Session *session = dynamic_cast<QInfinity::Session*>(sender());
    CollabDocument *collabDocument;

    if( !session )
    {
        kDebug() << "No sender in session sync slot.  (Should not happen)";
        return;
    }

    collabDocument = sessionCollabDocumentMap[*session];
    emit(documentLoaded(*collabDocument));
    kDebug() << "syncd";
}

void DocumentManager::setupSignals()
{
    connect( m_browserModel, SIGNAL(sessionSubscribed( QInfinity::BrowserNoteItem&, QInfinity::Session )),
        this, SLOT(slotSessionSubscribed( QInfinity::BrowserNoteItem&, QInfinity::Session )) );
}

void DocumentManager::insertSession( QInfinity::Session session )
{
    QInfinity::Session *localSession = new QInfinity::Session( session );
    KTextEditor::Document *document = editor->createDocument( this );
    CollabDocument *collabDocument = new CollabDocument( session, *document, this );
    m_collabDocuments << collabDocument;
    documentCollabDocumentMap[document] = collabDocument;
    sessionCollabDocumentMap[collabDocument->session()] = collabDocument;
    sessions << localSession;

    connect( localSession, SIGNAL(sessionSubscribed()), this, SLOT(slotSessionSubscribed()) );
    kDebug() << "insert";
}

}

