#include "documenttabwidget.h"

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

void DocumentTabWidget::closeWidget( QWidget *cw )
{
    int tab = indexOf( cw );
    removeTab( tab );
}

void DocumentTabWidget::addDocument( KTextEditor::Document &document )
{
    KTextEditor::View *view = document.createView( this );
    int tab;
    documentToView.insert( &document, view );
    tab = addTab( view, document.documentName() );
    setCurrentIndex( tab );
}

void DocumentTabWidget::removeDocument( KTextEditor::Document &document )
{
    int index = indexOf( documentView( document ) );
    removeTab( index );
}

KTextEditor::Document *DocumentTabWidget::documentAt( int index )
{
    return dynamic_cast<KTextEditor::Document*>(widget( index ));
}

KTextEditor::View *DocumentTabWidget::documentView( KTextEditor::Document &document )
{
    return documentToView[&document];
}

}
