#include "controlwidget.h"

namespace Kobby
{

ControlWidget::ControlWidget( InfinoteManager &manager, QWidget *parent )
    : QWidget( parent )
    , infinoteManager( &manager )
    , connectionManager( new ConnectionManager( *infinoteManager ) )
{
}

}
