#include "mainwindow.h"
#include "sidebar.h"
#include <libqinfinitymm/infinotemanager.h>
#include <libqinfinitymm/filebrowsermodel.h>

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

#include <QSplitter>
#include <QTreeView>

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
    saveConfig();
    delete configGeneralGroup;
}

void MainWindow::openControlDialog()
{
}

void MainWindow::openSettingsDialog()
{
}

void MainWindow::init()
{
    infinoteManager = QInfinity::InfinoteManager::instance( this );
    m_sidebar = new Sidebar( this );
    
    curr_document = editor->createDocument(0);
    curr_view = qobject_cast<KTextEditor::View*>( curr_document->createView( this ) );
    
    mainSplitter = new QSplitter( Qt::Horizontal, this );
    
    mainSplitter->addWidget( m_sidebar );
    mainSplitter->addWidget( curr_view );
    setCentralWidget( mainSplitter );
    setupActions();
    
    setXMLFile("kobbyui.rc");
    createShellGUI( true );
    
    guiFactory()->addClient( curr_view );
    
    loadConfig();
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

void MainWindow::loadConfig()
{
    configptr = KSharedConfig::openConfig();
    configGeneralGroup = new KConfigGroup( configptr.data(), "General" );

    setMinimumWidth( configGeneralGroup->readEntry( "width", 100 ) );
    setMinimumHeight( configGeneralGroup->readEntry( "height", 100 ) );
}

void MainWindow::saveConfig()
{
    configGeneralGroup->writeEntry( "width", width() );
    configGeneralGroup->writeEntry( "height", height() );
}

}
