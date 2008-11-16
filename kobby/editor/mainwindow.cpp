#include "mainwindow.h"
#include "sidebar.h"
#include "../dialogs/createconnectiondialog.h"

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

#include "../mainwindow.moc"

namespace Kobby
{

MainWindow::MainWindow( QWidget *parent )
    : KParts::MainWindow( parent, "Kobby" )
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

void MainWindow::slotCreateConnection()
{
    CreateConnectionDialog *dialog = new CreateConnectionDialog( this );
    dialog->setVisible( true );
}

void MainWindow::slotProxy( const QString &a, const QString &b, unsigned int )
{
    kDebug() << "ping";
}

void MainWindow::setupActions()
{
    newDocumentAction = actionCollection()->addAction( "file_new_document" );
    newDocumentAction->setText( "Document..." );
    newDocumentAction->setWhatsThis( "Create a new document." );

    newConnectionAction = actionCollection()->addAction( "file_new_connection" );
    newConnectionAction->setText( "Connection..." );
    newConnectionAction->setWhatsThis( "Create a new connection to an Infinote server." );
    connect( newConnectionAction, SIGNAL(triggered()), this, SLOT(slotCreateConnection()) );
    
    controlAction = actionCollection()->addAction( "tools_kobby_control" );
    controlAction->setText( "Kobby" );
    connect( controlAction, SIGNAL(triggered()), this, SLOT(openControlDialog()) );
    
    settingsAction = actionCollection()->addAction( "settings_kobby" );
    settingsAction->setText( "Configure Kobby" );
    connect( settingsAction, SIGNAL(triggered()), this, SLOT(openSettingsDialog()) );
}

void MainWindow::loadConfig()
{
    QList<int> s;
    configptr = KSharedConfig::openConfig();
    configGeneralGroup = new KConfigGroup( configptr.data(), "General" );

    setMinimumWidth( configGeneralGroup->readEntry( "width", 100 ) );
    setMinimumHeight( configGeneralGroup->readEntry( "height", 100 ) );
    s << 1 << 5;
    s = configGeneralGroup->readEntry( "sidebarSizes", s );
    if( s.size() > 0 )
        mainSplitter->setSizes( s );
}

void MainWindow::saveConfig()
{
    configGeneralGroup->writeEntry( "width", width() );
    configGeneralGroup->writeEntry( "height", height() );
    configGeneralGroup->writeEntry( "sidebarSizes", mainSplitter->sizes() );
}

}
