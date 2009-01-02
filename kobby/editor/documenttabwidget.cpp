#include "documenttabwidget.h"

#include <KIcon>
#include <KTextEditor/Document>
#include <KTextEditor/View>

#include <QToolButton>

namespace Kobby
{

DocumentTabWidget::DocumentTabWidget( QWidget *parent )
    : QTabWidget( parent )
{
    closeButton = new QToolButton();
    closeButton->setIcon( KIcon( "tab-close.png" ) );
    setCornerWidget( closeButton, Qt::TopRightCorner );
}

DocumentTabWidget::~DocumentTabWidget()
{
    delete closeButton;
}

void DocumentTabWidget::addDocument( KTextEditor::Document &document )
{
    addTab( document.createView( this ), document.documentName() );
}

}
