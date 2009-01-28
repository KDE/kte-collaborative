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

void DocumentManager::slotSessionSubscribed( QInfinity::BrowserNoteItem &note )
{
    QInfinity::Session *session = note.session();

    if( session )
        insertSession( *session );    
    else
    {
        kDebug() << "Note subscribed to but cant find session in note item!";
    }
}

void DocumentManager::setupSignals()
{
    connect( m_browserModel, SIGNAL(sessionSubscribed( QInfinity::BrowserNoteItem& )),
        this, SLOT(slotSessionSubscribed( QInfinity::BrowserNoteItem& )) );
}

void DocumentManager::insertSession( QInfinity::Session &session )
{
    KTextEditor::Document *document = editor->createDocument( this );
    CollabDocument *collabDocument = new CollabDocument( session, *document, this );
    m_collabDocuments << collabDocument;
    documentCollabDocumentMap[document] = collabDocument;

    emit(documentLoading( *collabDocument ));
}

}

