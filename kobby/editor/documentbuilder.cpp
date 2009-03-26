#include "documentbuilder.h"
#include "document.h"

#include <libqinfinity/browsermodel.h>

#include <KTextEditor/Editor>
#include <KTextEditor/Document>
#include <KUrl>
#include <KDebug>

#include "documentbuilder.moc"

namespace Kobby
{

DocumentBuilder::DocumentBuilder( KTextEditor::Editor &editor,
    QInfinity::BrowserModel &browserModel,
    QObject *parent )
    : QObject( parent )
    , editor( &editor )
    , m_browserModel( &browserModel )
{
}

DocumentBuilder::~DocumentBuilder()
{
}

void DocumentBuilder::openBlank()
{
    Document *doc = new Document( *editor->createDocument( this ), this );
    emit( documentCreated( *doc ) );
}

void DocumentBuilder::openInfDocmuent( const QModelIndex &index )
{
}

void DocumentBuilder::openUrl( const KUrl &url )
{
    KTextEditor::Document *kdoc = editor->createDocument( this );
    Document *doc;
    kdoc->openUrl( url );
    doc = new Document( *kdoc, this );
    emit( documentCreated( *doc ) );
}

}

