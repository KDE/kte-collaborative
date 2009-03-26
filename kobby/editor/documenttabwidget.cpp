#include "documenttabwidget.h"
#include "document.h"

#include <KIcon>
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KDebug>

#include <QToolButton>

#include "documenttabwidget.moc"

namespace Kobby
{

DocumentTabWidget::DocumentTabWidget( QWidget *parent )
    : KTabWidget( parent )
{
    connect( this, SIGNAL(closeRequest( QWidget* )),
        this, SLOT(closeWidget( QWidget* )) );
    setCloseButtonEnabled( true );
}

DocumentTabWidget::~DocumentTabWidget()
{
}

KTextEditor::View *DocumentTabWidget::viewAt( int index )
{
    return dynamic_cast<KTextEditor::View*>(widget( index ));
}

void DocumentTabWidget::addDocument( Document &doc )
{
    addDocument( *doc.kDocument() );
}

void DocumentTabWidget::removeDocument( Document &doc )
{
}

void DocumentTabWidget::closeWidget( QWidget *cw )
{
    int tab = indexOf( cw );
    removeTab( tab );
}

void DocumentTabWidget::addDocument( KTextEditor::Document &document )
{
    KTextEditor::View *view = document.createView( this );
    int tab;
    tab = addTab( view, document.documentName() );
    setCurrentIndex( tab );
}

void DocumentTabWidget::removeDocument( KTextEditor::Document &document )
{
}

}
