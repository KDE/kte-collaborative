#include <kobby/editor/mainwindow.h>
#include <kobby/infinote/infinotemanager.h>
#include <kobby/dialogs/controldialog.h>

#include <KAction>
#include <KActionCollection>
#include <KApplication>
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
    KTextEditor::Editor *editor = KTextEditor::EditorChooser::editor();
    
    if( !editor )
    {
        KMessageBox::error(this, i18n("A KDE text-editor component could not be found;\n"
            "please check your KDE installation."));
        kapp->exit(1);
    }
    
    // Initialize Infinote
    infinoteManager = new InfinoteManager( this );
    
    curr_document = editor->createDocument(0);
    curr_view = qobject_cast<KTextEditor::View*>( curr_document->createView( this ) );
    
    setCentralWidget( curr_view );
    setupActions();
    
    setXMLFile("kobbyui.rc");
    createShellGUI( true );
    
    guiFactory()->addClient( curr_view );
    
    show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupActions()
{
    controlAction = actionCollection()->addAction( "tools_kobby_control" );
    controlAction->setText( "Kobby" );
    connect( controlAction, SIGNAL(triggered()), this, SLOT(openControlDialog()) );
}

void MainWindow::openControlDialog()
{
    ControlDialog *dialog = new ControlDialog( *infinoteManager, this );
    dialog->setVisible( true );
}

}
