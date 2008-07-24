#include "configdialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

namespace Kobby
{

ConfigDialogList::ConfigDialogList()
    : QListWidget()
{
    userItem.setText( "User" );
    addItem( &userItem );
    
    sessionsItem.setText( "Sessions" );
    addItem( &sessionsItem );
    
    connect(this, SIGNAL( selectionChanged() ), this, SLOT( slotItemSectionChanged() ) );
}

void ConfigDialogList::slotItemSelectionChanged()
{
    QListWidgetItem *selectedItem = currentItem();
    
    if( selectedItem == &userItem )
        emit( itemSelected( ConfigDialogList::User ) );
    else if( selectedItem == &sessionsItem )
        emit( itemSelected( ConfigDialogList::Sessions ) );
}

ConfigDialog::ConfigDialog( QWidget *parent, const QVariantList &args )
    : KCModule( PluginFactory::componentData(), parent, args )
{
    
}

} // namespace Kobby

#include "configdialog.moc"
