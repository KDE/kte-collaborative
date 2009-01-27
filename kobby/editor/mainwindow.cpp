#include <libinfinitymm/client/clientbrowser.h>
#include <libinfinitymm/client/clientsessionproxy.h>
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
#include "kobbysettings.h"

#include <libqinfinitymm/infinotemanager.h>
#include <libqinfinitymm/browseritem.h>
#include <libqinfinitymm/browsermodel.h>
#include <libqinfinitymm/document.h>

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
#include <QTabWidget>

#include <mainwindow.moc>

namespace Kobby
{

MainWindow::MainWindow( QWidget *parent )
    : infinoteManager( QInfinity::InfinoteManager::instance() )
    , browserModel( new BrowserModel( this ) )
    , documentTab( new DocumentTabWidget( this ) )
    , curr_collabDocument( 0 )
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
    NotePlugin *notePlugin = new NotePlugin();
    infinoteManager->addNotePlugin( *notePlugin );
}

MainWindow::~MainWindow()
{
    saveConfig();
    delete documentTab;
    delete browserModel;
    QList<CollabDocument*>::Iterator itr;
    for( itr = collabDocuments.begin(); itr != collabDocuments.end(); itr++ )
    {
        delete *itr;
    }
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

void MainWindow::slotSessionSubscribed( QInfinity::BrowserNoteItem &node,
    Glib::RefPtr<Infinity::ClientSessionProxy> sessionProxy )
{
    Q_UNUSED(node)
    kDebug() << "Subscribed to new session.";
    Infinity::Session *session = sessionProxy->getSession();
    if( !session )
    {
        kDebug() << "Could not get session from session proxy.";
        return;
    }
    curr_collabDocument = new CollabDocument( sessionProxy, *editor->createDocument( this ), editor );
    curr_document = curr_collabDocument->kDocument();
    documentTab->addDocument( *curr_document );
    collabDocuments.append( curr_collabDocument );
    collabDocumentMap.insert( curr_document, curr_collabDocument );
}

void MainWindow::slotDocumentTabChanged( int index )
{
    KTextEditor::Document *newDocument = documentTab->documentAt( index );
    curr_document = newDocument;
    curr_view = documentTab->documentView( *curr_document );
    curr_collabDocument = collabDocumentMap[curr_document];
}

void MainWindow::slotDocumentClose( KTextEditor::Document *document )
{
    Q_UNUSED(document)
}

void MainWindow::setupUi()
{
    connectionManager = new ConnectionManagerWidget( this );
    fileBrowser = new FileBrowserWidget( *browserModel, this );
    m_sidebar = new Sidebar( this );
    m_sidebar->addTab( connectionManager, "Connections" );
    m_sidebar->addTab( fileBrowser, "Browse" );

    curr_document = editor->createDocument(0);
    documentTab->addDocument( *curr_document );
    curr_view = curr_document->activeView();
    
    mainSplitter = new QSplitter( Qt::Horizontal, this );
    
    mainSplitter->addWidget( m_sidebar );
    mainSplitter->addWidget( documentTab );
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

    // Connect to InfinoteManager
    connect( infinoteManager, SIGNAL(connectionAdded( Connection& )),
        this, SLOT(addConnection( Connection& )) );

    // Connect to BrowserModel
    connect( browserModel, SIGNAL(sessionSubscribed( QInfinity::BrowserNoteItem&,
            Glib::RefPtr<Infinity::ClientSessionProxy> )),
        this, SLOT(slotSessionSubscribed( QInfinity::BrowserNoteItem&,
            Glib::RefPtr<Infinity::ClientSessionProxy> )) );

    // Connect to DocumentTabWidget
    connect( documentTab, SIGNAL(currentChanged( int )), this, SLOT(documentTabChanged( int )) );
    connect( documentTab, SIGNAL(documentClose( KTextEditor::Document* )),
        this, SLOT(slotDocumentClose( KTextEditor::Document* )) );
}

void MainWindow::loadConfig()
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

void MainWindow::saveConfig()
{
    QList<int> sizes;

    sizes << x() << y() << width() << height();
    KobbySettings::setMainWindowGeometry( sizes );
    KobbySettings::setMainWindowSplitterSizes( mainSplitter->sizes() );
    KobbySettings::self()->writeConfig();
}

}
