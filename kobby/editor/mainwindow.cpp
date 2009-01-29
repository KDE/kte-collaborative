#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/common/session.h>
#include <libinfinitymm/common/buffer.h>
#include <libinftextmm/textbuffer.h>

// This has to be included before anything using Qt
#include "noteplugin.h"

#include "mainwindow.h"
#include "sidebar.h"
#include "createconnectiondialog.h"
#include "browsermodel.h"
#include "filebrowserwidget.h"
#include "connectionmanagerwidget.h"
#include "collabdocument.h"
#include "documenttabwidget.h"
#include "documentmanager.h"
#include "kobbysettings.h"
#include "profilesettings.h"

#include <libqinfinitymm/infinotemanager.h>
#include <libqinfinitymm/browseritem.h>
#include <libqinfinitymm/browsermodel.h>
#include <libqinfinitymm/document.h>

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KConfigDialog>
#include <KLocale>
#include <KMessageBox>
#include <KXMLGUIFactory>
#include <KDebug>
#include <KPageWidgetItem>

#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/EditorChooser>

#include <QSplitter>
#include <QTreeView>
#include <QTabWidget>

#include <mainwindow.moc>

namespace Kobby
{

MainWindow::MainWindow( QWidget *parent )
    : infinoteManager( QInfinity::InfinoteManager::instance() )
    , browserModel( new BrowserModel( this ) )
    , documentTab( new DocumentTabWidget( this ) )
{
    Q_UNUSED(parent)
    
    editor = KTextEditor::EditorChooser::editor();
    if( !editor )
    {
        KMessageBox::error(this, i18n("A KDE text-editor component could not be found;\n"
            "please check your KDE installation."));
        kapp->exit(1);
    }
    
    documentManager = new DocumentManager( *editor, *browserModel );
    setupUi();
    setupSignals();
    NotePlugin *notePlugin = new NotePlugin();
    infinoteManager->addNotePlugin( *notePlugin );
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete documentTab;
    delete browserModel;
}

void MainWindow::slotCreateConnection()
{
    CreateConnectionDialog *dialog = new CreateConnectionDialog( this );
    dialog->setVisible( true );
}

void MainWindow::slotOpenItem( QInfinity::BrowserItem &item )
{
    Q_UNUSED(item)
}

void MainWindow::setupUi()
{
    KTextEditor::Document *document;

    connectionManager = new ConnectionManagerWidget( this );
    fileBrowser = new FileBrowserWidget( *browserModel, this );
    m_sidebar = new Sidebar( this );
    m_sidebar->addTab( connectionManager, "Connections" );
    m_sidebar->addTab( fileBrowser, "Browse" );

    document = editor->createDocument( this );
    documentTab->addDocument( *document );
    
    mainSplitter = new QSplitter( Qt::Horizontal, this );
    
    mainSplitter->addWidget( m_sidebar );
    mainSplitter->addWidget( documentTab );
    setCentralWidget( mainSplitter );
    
    setupGUI( (ToolBar | Keys | StatusBar | Save), "kobbyui.rc");
    setupActions();
    createShellGUI( true );
    
    guiFactory()->addClient( document->activeView() );
    
    loadSettings();
}

void MainWindow::setupActions()
{
    // Setup menu actions
    newDocumentAction = actionCollection()->addAction( "file_new_document" );
    newDocumentAction->setText( "Document..." );
    newDocumentAction->setIcon( KIcon( "document-new.png" ) );
    newDocumentAction->setWhatsThis( "Create a new document." );

    newConnectionAction = actionCollection()->addAction( "file_new_connection" );
    newConnectionAction->setText( "Connection..." );
    newConnectionAction->setIcon( KIcon( "network-connect.png" ) );
    newConnectionAction->setWhatsThis( "Create a new connection to an Infinote server." );

    settingsAction = actionCollection()->addAction( "settings_kobby" );
    settingsAction->setText( "Configure Kobby..." );
    settingsAction->setWhatsThis( "Modify kobby settings." );
}

void MainWindow::setupSignals()
{
    connect( newConnectionAction, SIGNAL(triggered()), this, SLOT(slotCreateConnection()) );
    connect( settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()) );

    // Connect to InfinoteManager
    connect( infinoteManager, SIGNAL(connectionAdded( Connection& )),
        this, SLOT(addConnection( Connection& )) );

    connect( documentManager, SIGNAL(documentLoading( CollabDocument& )),
        this, SLOT(documentLoading( CollabDocument& )) );

    connect( editor, SIGNAL(documentCreated( KTextEditor::Editor*, KTextEditor::Document* )),
        this, SLOT(documentCreated( KTextEditor::Editor*, KTextEditor::Document * )) );
}

void MainWindow::loadSettings()
{
    QList<int> sizes;

    sizes = KobbySettings::mainWindowGeometry();
    if( sizes.size() == 4 )
        setGeometry( sizes[0], sizes[1], sizes[2], sizes[3] );
    sizes = KobbySettings::mainWindowSplitterSizes();
    if( sizes.size() )
        mainSplitter->setSizes( sizes );
    else
    {
        sizes.empty();
        sizes << 1 << 5;
        mainSplitter->setSizes( sizes );
    }
}

void MainWindow::saveSettings()
{
    QList<int> sizes;

    sizes << x() << y() << width() << height();
    KobbySettings::setMainWindowGeometry( sizes );
    KobbySettings::setMainWindowSplitterSizes( mainSplitter->sizes() );
    KobbySettings::self()->writeConfig();
}

void MainWindow::showSettingsDialog()
{
    KPageWidgetItem *item;

    if( KConfigDialog::showDialog("Kobby Settings") )
        return;

    KConfigDialog *dialog = new KConfigDialog( this, "Kobby Settings", KobbySettings::self() );
    item = dialog->addPage( new ProfileSettings, "Profile", "Profile" );
    item->setIcon( KIcon( "user-identity.png" ) );
    dialog->show();
}

void MainWindow::documentLoading( CollabDocument &document )
{
    Q_UNUSED(document)
}

void MainWindow::documentCreated( KTextEditor::Editor *editor, KTextEditor::Document *document )
{
    Q_UNUSED(editor)
    documentTab->addDocument( *document );
}

}
