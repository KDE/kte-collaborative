#include "documentbuilder.h"

#include <libqinfinity/browsermodel.h>

#include <KTextEditor/Editor>
#include <KTextEditor/Document>
#include <KUrl>
#include <KDebug>

#include "documentbuilder.moc"

namespace Kobby
{

DocumentBuilder::DocumentBuilder( KTextEditor::Editor &editor,
    QInfinity::BrowserModel &browserModel )
    : editor( &editor )
    , m_browserModel( &browserModel )
{
    setupSignals();
}

DocumentBuilder::~DocumentBuilder()
{
}

void DocumentBuilder::openInfDocmuent( const QModelIndex &index )
{
}

void DocumentBuilder::openUrl( const KUrl &url )
{
}

void DocumentBuilder::setupSignals()
{
}

}

