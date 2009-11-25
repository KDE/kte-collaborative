/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "kobbysettings.h"
#include "settingsdialog.h"
#include "setupdialog.h"
#include "remotebrowserview.h"
#include "localbrowserview.h"
#include "documentusersbrowser.h"
#include "documentlistview.h"
#include "documenttabwidget.h"
#include "documentmodel.h"
#include "documentbuilder.h"
#include "document.h"
#include "connection.h"
#include "createconnectiondialog.h"
#include "itemfactory.h"
#include "noteplugin.h"

#include <libqinfinity/defaulttextplugin.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browseritemfactory.h>
#include <libqinfinity/xmppconnection.h>

#include <KApplication>
#include <KAction>
#include <KActionCollection>
#include <KConfigDialog>
#include <KXMLGUIFactory>
#include <KLocalizedString>
#include <KMessageBox>
#include <KFileDialog>
#include <KUrl>

#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/EditorChooser>
#include <KTextEditor/Document>

#include <QLabel>
#include <QSplitter>
#include <QTreeView>
#include <QTabWidget>
#include <QStatusBar>
#include <QHostInfo>
#include <QToolBox>

#include <KDebug>

#include "mainwindow.moc"

namespace Kobby
{

MainWindow::MainWindow( QWidget *parent )
    : mergedTextView( 0 )
{
    Q_UNUSED( parent );

    editor = KTextEditor::EditorChooser::editor();
    if( !editor )
    {
        KMessageBox::error(this, i18n("A KDE text-editor component could not be found;\n"
            "please check your KDE installation."));
        kapp->exit(1);
    }

    browserModel = new QInfinity::BrowserModel( this );
    browserModel->setItemFactory( new ItemFactory( this ) );

    // Setup Document Management
    docTabWidget = new DocumentTabWidget( this );
    docModel = DocumentModel::instance();
    docBuilder = new DocumentBuilder( *editor, *browserModel, this );
    connect( docBuilder, SIGNAL(documentCreated(Document&)),
        docModel, SLOT(insertDocument(Document&)) );
    connect( docModel, SIGNAL(documentAdded(Document&)),
        docTabWidget, SLOT(addDocument(Document&)) );
    connect( docModel, SIGNAL(documentAboutToBeRemoved(Document&)),
        docTabWidget, SLOT(removeDocument(Document&)) );
    connect( docModel, SIGNAL(documentFatalError( Document*, QString )),
        this, SLOT(slotDocumentFatalError( Document*, QString )) );
    connect( docTabWidget, SIGNAL(viewActivated( KTextEditor::View * )),
        this, SLOT(slotTextViewActivated( KTextEditor::View * )) );

    // Setup the QInfinity BrowserModel
    textPlugin = new NotePlugin( *docBuilder, this );
    browserModel->addPlugin( *textPlugin );

    setXMLFile( "kobbyui.rc" );
    setupUi();
    setupActions();
    createShellGUI( true );

    restoreSettings();

    if( KobbySettings::blankDocumentOnStart() )
    {
        // Open blank document on start
        docBuilder->openBlank();
    }

    if( needsSetupDialog() )
    {
        SetupDialog *dialog = new SetupDialog( this );
        if( !dialog->exec() )
            deleteLater();
    }

    if( KobbySettings::connectDialogOnStart() )
    {
        // Start with connection dialog open
        slotNewConnection();
    }
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete docTabWidget;
    delete usersBrowser;
    delete docModel;
}

void MainWindow::setupUi()
{
    setWindowIcon( KIcon( "meeting-attending.png" ) );

    statusLabel = new QLabel( this );
    QStatusBar *statusBar = new QStatusBar( this );
    statusBar->addWidget( statusLabel );
    setStatusBar( statusBar );

    // Setup UsersBrowser
    usersBrowser = new DocumentUsersBrowser( this );

    // Setup RemoteBrowserView
    remoteBrowserView = new RemoteBrowserProxy( *textPlugin,
        *browserModel, this );
    connect( &remoteBrowserView->remoteView(), SIGNAL(createConnection()),
        this, SLOT(slotNewConnection()) );
    connect( &remoteBrowserView->remoteView(), SIGNAL(openItem(const QModelIndex&)),
        docBuilder, SLOT(openInfDocmuent(const QModelIndex&)) );
    connect( remoteBrowserView, SIGNAL(createConnection()),
        this, SLOT(slotNewConnection()) );
    connect( remoteBrowserView, SIGNAL(createConnection(const QString&, unsigned int)),
        this, SLOT(slotCreateConnection(const QString&, unsigned int)) );

    // Setup LocalBrowserView
    localBrowserView = new LocalBrowserView( this );
    connect( localBrowserView, SIGNAL(urlSelected(const KUrl&)),
        docBuilder, SLOT(openUrl(const KUrl&)) );

    documentListView = new DocumentListView( *docModel, this );
    connect( documentListView, SIGNAL(documentActivated(Document&)),
        docTabWidget, SLOT(addDocument(Document&)) );

    // Setup Left ToolBox
    leftToolBox = new QToolBox( this );
    leftToolBox->addItem( documentListView,
        KIcon("document-preview.png"),
        i18n("Documents") );
    leftToolBox->addItem( remoteBrowserView,
        KIcon("document-open-remote.png"),
        i18n("Remote Browser") );
    leftToolBox->addItem( localBrowserView,
        KIcon("folder.png"),
        i18n("Local Browser") );
    leftToolBox->addItem( usersBrowser,
        KIcon("meeting-organizer.png"),
        i18n("Users") );
    leftToolBox->setCurrentWidget( remoteBrowserView );

    mainHorizSplitter = new QSplitter( Qt::Horizontal, this );
    mainHorizSplitter->addWidget( leftToolBox );
    mainHorizSplitter->addWidget( docTabWidget );
    mainHorizSplitter->setStretchFactor( 0, QSizePolicy::Fixed );

    setCentralWidget( mainHorizSplitter );
    setAutoSaveSettings( "MainWindow", true );
}

void MainWindow::setupActions()
{
    /* TODO:
       Close action is unconnected
     */
    newConnectionAction = new KAction( i18n("New Connection"), this );
    newConnectionAction->setWhatsThis( i18n("Create a new connection.") );
    settingsAction = KStandardAction::preferences( this, SLOT(slotShowSettingsDialog()),
        actionCollection() );
    settingsAction->setWhatsThis( i18n( "Kobby settings." ) );

    newConnectionAction->setIcon( KIcon("network-connect.png") );

    connect( newConnectionAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewConnection()) );

    KStandardAction::quit(kapp, SLOT(quit()),
        actionCollection());

    actionCollection()->addAction( KStandardAction::New, "document_new",
        docBuilder, SLOT(openBlank()) )->setWhatsThis( i18n( "Open a new blank document." ) );
    actionCollection()->addAction( "connection_new", newConnectionAction );
    actionCollection()->addAction( KStandardAction::Open, "document_open",
        this, SLOT(slotOpenFile()) )->setWhatsThis( i18n("Select a document to open.") );
    actionCollection()->addAction( KStandardAction::Close, "document_close",
        this, SLOT(slotCloseActive()) )->setWhatsThis( i18n("Close active document.") );
}

void MainWindow::slotNewConnection()
{
    CreateConnectionDialog *dialog = new CreateConnectionDialog( this );
    connect( dialog, SIGNAL(createConnection(const QString&, unsigned int)),
        this, SLOT(slotCreateConnection(const QString&, unsigned int)) );
    dialog->setVisible( true );
}

void MainWindow::slotOpenFile()
{
    KFileDialog *dialog = new KFileDialog( KUrl("~"), "", this );
    dialog->setMode( KFile::Files | KFile::ExistingOnly );
    if( dialog->exec() )
    {
        docBuilder->openUrl( dialog->selectedUrl() );
    }
    delete dialog;
}

void MainWindow::slotCreateConnection( const QString &hostname,
    unsigned int port )
{
    Connection *conn = new Connection( hostname, port, 0 );
    connect( conn, SIGNAL(connected(Connection*)),
        this, SLOT(slotConnectionConnected(Connection*)) );
    connect( conn, SIGNAL(error(Connection*, QString)),
        this, SLOT(slotConnectionError(Connection*, QString)) );
    conn->open();
}

void MainWindow::slotConnectionError( Connection *conn,
    QString errMsg )
{
    Q_UNUSED(conn);
    QString str = i18n("Connection error: %1", errMsg);
    KMessageBox::error( this, str );
    slotNewConnection();
}

void MainWindow::slotCloseActive()
{
    KTextEditor::View *activeView = docTabWidget->activeView();
    if( activeView )
    {
        docModel->removeDocument( *activeView->document() );
    }
}

void MainWindow::slotTextViewActivated( KTextEditor::View *view )
{
    // Make sure the correct view is merged
    if( mergedTextView && mergedTextView != view )
    {
        // We need to unmerge current view
        guiFactory()->removeClient( mergedTextView );
    }
    
    if( view )
    {
        // Merge new view
        guiFactory()->addClient( view );
        Document *activeDoc = docModel->documentFromKDoc( *view->document() );
        usersBrowser->setActiveDocument( activeDoc );
    }
    else
    {
        usersBrowser->setActiveDocument( 0 );
    }
    mergedTextView = view;
}

void MainWindow::slotDocumentFatalError( Kobby::Document* doc, QString message )
{
    Q_UNUSED(doc)
    QString str = i18n("An error has occoured with the document: %1", message);
    KMessageBox::error( this, str );
}

void MainWindow::slotShowSettingsDialog()
{
    if( KConfigDialog::showDialog(i18n("Kobby Settings")) )
        return;

    SettingsDialog *dialog = new SettingsDialog( this );
    dialog->show();
}

void MainWindow::slotConnectionConnected( Connection *conn )
{
    QInfinity::ConnectionItem *item;
    // We are using our subclassed QInfinity ConnectionItem
    item = browserModel->addConnection( *conn->xmppConnection(), conn->name() );
    dynamic_cast<Kobby::ConnectionItem*>(item)->setConnection( conn );
}

void MainWindow::restoreSettings()
{
    QList<int> sizes;
    sizes = KobbySettings::mainWindowHorizSplitterSizes();
    if( sizes.size() )
        mainHorizSplitter->setSizes( sizes );
    else
    {
        sizes.empty();
        sizes << 200 << 450;
        mainHorizSplitter->setSizes( sizes );
    }

    if( KobbySettings::hostName().isEmpty() )
        KobbySettings::setHostName( QHostInfo::localHostName() );
}

void MainWindow::saveSettings()
{
    QList<int> sizes;
    KobbySettings::setMainWindowHorizSplitterSizes( mainHorizSplitter->sizes() );
    KobbySettings::self()->writeConfig();
}

Document *MainWindow::activeDocument()
{
    if( mergedTextView )
    {
        return docModel->documentFromKDoc( *mergedTextView->document() );
    }
    else
        return 0;
}

bool MainWindow::needsSetupDialog()
{
    if( KobbySettings::nickName().isEmpty() )
        return true;
    else
        return false;
}

}
