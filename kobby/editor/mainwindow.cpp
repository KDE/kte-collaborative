#include <libinfinitymm/client/clientbrowser.h>

#include "mainwindow.h"
#include "sidebar.h"
#include "createconnectiondialog.h"
#include "filebrowserwidget.h"
#include "connectionmanagerwidget.h"

#include <libqinfinitymm/infinotemanager.h>
#include <libqinfinitymm/filebrowseritem.h>
#include <libqinfinitymm/filebrowsermodel.h>

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocale>
#include <KMessageBox>
#include <KXMLGUIFactory>
#include <KDebug>

#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/EditorChooser>

#include <QSplitter>
#include <QTreeView>

#include "mainwindow.moc"

namespace Kobby
{

MainWindow::MainWindow( QWidget *parent )
    : infinoteManager( QInfinity::InfinoteManager::instance() )
{
    Q_UNUSED(parent)
    
    editor = KTextEditor::EditorChooser::editor();
    
    if( !editor )
    {
        KMessageBox::error(this, i18n("A KDE text-editor component could not be found;\n"
            "please check your KDE installation."));
        kapp->exit(1);
    }
    
    setupUi();
    setupActions();
}

MainWindow::~MainWindow()
{
    saveConfig();
    delete configGeneralGroup;
}

void MainWindow::slotCreateConnection()
{
    CreateConnectionDialog *dialog = new CreateConnectionDialog( this );
    dialog->setVisible( true );
}

void MainWindow::slotOpenItem( QInfinity::FileBrowserItem &item )
{
    item.open();
}

void MainWindow::addConnection( QInfinity::Connection &connection )
{
    // Let us know when weve subscribed to a note
    Infinity::ClientBrowser *browser;
    browser = connection.clientBrowser();
    if( browser )
    {
    }
}

void MainWindow::setupUi()
{
    connectionManager = new ConnectionManagerWidget( this );
    fileBrowser = new FileBrowserWidget( this );
    m_sidebar = new Sidebar( this );
    m_sidebar->addTab( connectionManager, "Connections" );
    m_sidebar->addTab( fileBrowser, "Browse" );

    curr_document = editor->createDocument(0);
    curr_view = qobject_cast<KTextEditor::View*>( curr_document->createView( this ) );
    
    mainSplitter = new QSplitter( Qt::Horizontal, this );
    
    mainSplitter->addWidget( m_sidebar );
    mainSplitter->addWidget( curr_view );
    setCentralWidget( mainSplitter );
    
    setXMLFile("kobbyui.rc");
    createShellGUI( true );
    
    guiFactory()->addClient( curr_view );
    
    loadConfig();
}

void MainWindow::setupActions()
{
    // Setup menu actions
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

    // Connect to InfinoteManager
    connect( infinoteManager, SIGNAL(connectionAdded( Connection& )),
        this, SLOT(addConnection( Connection& )) );

    connect( fileBrowser, SIGNAL(itemOpened( QInfinity::FileBrowserItem& )),
        this, SLOT(slotOpenItem( QInfinity::FileBrowserItem& )) );
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
