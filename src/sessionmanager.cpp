#include "sessionmanager.h"

namespace Kobby
{

SessionManager::SessionManager( QWidget *parent )
    : KDialog( parent )
{
    QWidget *widget = new QWidget( this );
    ui.setupUi( widget );
    setMainWidget( widget );
    
    setCaption( i18n("Kobby Session Manager") );
}

} // namespace Kobby
