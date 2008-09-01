#include "configdialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>

namespace Kobby
{

ConfigDialogList::ConfigDialogList()
    : QListWidget()
{
    userItem.setText( "User" );
    addItem( &userItem );
    
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
    QHBoxLayout *layout = new QHBoxLayout;
    QSplitter *splitter = new QSplitter( Qt::Horizontal, this );
    
    splitter->addWidget( new ConfigDialogList );
    splitter->addWidget( &tabWidget );

    layout->addWidget( splitter );

    setLayout( layout );
}

} // namespace Kobby

#include "configdialog.moc"
