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
    : QTabWidget( parent )
{
    closeButton = new QToolButton();
    closeButton->setIcon( KIcon( "tab-close.png" ) );
    setCornerWidget( closeButton, Qt::TopRightCorner );

    connect( closeButton, SIGNAL(pressed()), this, SLOT(closeCurrentTab()) );
}

DocumentTabWidget::~DocumentTabWidget()
{
    delete closeButton;
}

void DocumentTabWidget::closeCurrentTab()
{
    emit(closingDocument( documentAt( currentIndex() ) ));
    removeTab( currentIndex() );
}

void DocumentTabWidget::addDocument( KTextEditor::Document &document )
{
    addTab( document.createView( this ), document.documentName() );
}

KTextEditor::Document *DocumentTabWidget::documentAt( int index )
{
    return dynamic_cast<KTextEditor::Document*>(widget( index ));
}

}
