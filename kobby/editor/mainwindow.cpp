#include <kobby/editor/mainwindow.h>
#include <libqinfinitymm/infinotemanager.h>

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocale>
#include <KMessageBox>
#include <KXMLGUIFactory>

#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/EditorChooser>

namespace Kobby
{

MainWindow::MainWindow( QWidget *parent )
{
    Q_UNUSED(parent)
    
    editor = KTextEditor::EditorChooser::editor();
    
    if( !editor )
    {
        KMessageBox::error(this, i18n("A KDE text-editor component could not be found;\n"
            "please check your KDE installation."));
        kapp->exit(1);
    }
    
    init();
}

MainWindow::~MainWindow()
{
    configGeneralGroup->writeEntry( "width", width() );
    configGeneralGroup->writeEntry( "height", height() );
    
    delete configGeneralGroup;
}

void MainWindow::init()
{
    configptr = KSharedConfig::openConfig();
    configGeneralGroup = new KConfigGroup( configptr.data(), "General" );
    
    // Initialize Infinote
    infinoteManager = QInfinity::InfinoteManager::instance( this );
    
    curr_document = editor->createDocument(0);
    curr_view = qobject_cast<KTextEditor::View*>( curr_document->createView( this ) );
    
    setCentralWidget( curr_view );
    setupActions();
    
    setXMLFile("kobbyui.rc");
    createShellGUI( true );
    
    guiFactory()->addClient( curr_view );
    
    setMinimumWidth( configGeneralGroup->readEntry( "width", 100 ) );
    setMinimumHeight( configGeneralGroup->readEntry( "height", 100 ) );
}

void MainWindow::setupActions()
{
    controlAction = actionCollection()->addAction( "tools_kobby_control" );
    controlAction->setText( "Kobby" );
    connect( controlAction, SIGNAL(triggered()), this, SLOT(openControlDialog()) );
    
    settingsAction = actionCollection()->addAction( "settings_kobby" );
    settingsAction->setText( "Configure Kobby" );
    connect( settingsAction, SIGNAL(triggered()), this, SLOT(openSettingsDialog()) );
}

void MainWindow::openControlDialog()
{
}

void MainWindow::openSettingsDialog()
{
}

}
