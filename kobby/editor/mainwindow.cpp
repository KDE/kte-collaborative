#include "mainwindow.h"
#include "kobbysettings.h"
#include "settingsdialog.h"
#include "remotebrowserview.h"
#include "localbrowserview.h"
#include "documenttabwidget.h"
#include "connection.h"
#include "createconnectiondialog.h"
#include "itemfactory.h"

#include <libqinfinity/browseritemfactory.h>
#include <libqinfinity/xmppconnection.h>

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
#include <KLocalizedString>
#include <KTabWidget>
#include <KMessageBox>

#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/EditorChooser>
#include <KTextEditor/Document>

#include <QLabel>
#include <QSplitter>
#include <QTreeView>
#include <QTabWidget>
#include <QStatusBar>

#include <QDebug>

#include "mainwindow.moc"

namespace Kobby
{

MainWindow::MainWindow( QWidget *parent )
{
    Q_UNUSED( parent )

    editor = KTextEditor::EditorChooser::editor();
    if( !editor )
    {
        KMessageBox::error(this, i18n("A KDE text-editor component could not be found;\n"
            "please check your KDE installation."));
        kapp->exit(1);
    }

    browserModel = new QInfinity::BrowserModel( this );
    browserModel->setItemFactory( new ItemFactory( this ) );
    docTabWidget = new DocumentTabWidget( this );

    setXMLFile( "kobbyui.rc" );

    KTextEditor::Document *doc = editor->createDocument( this );
    docTabWidget->addDocument( *doc );

    setupActions();
    createShellGUI( true );
    guiFactory()->addClient( docTabWidget->documentView( *doc ) );
    setupUi();

    restoreSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUi()
{
    statusLabel = new QLabel( this );
    QStatusBar *statusBar = new QStatusBar( this );
    statusBar->addWidget( statusLabel );
    setStatusBar( statusBar );

    remoteBrowserView = new RemoteBrowserView( *browserModel, this );
    connect( remoteBrowserView, SIGNAL(createConnection()),
        this, SLOT(slotNewConnection()) );
    localBrowserView = new LocalBrowserView( this );

    leftTabWidget = new KTabWidget( this );
    leftTabWidget->setTabPosition( QTabWidget::West );
    leftTabWidget->addTab( remoteBrowserView,
        KIcon("document-open-remote.png"),
        i18n("Remote Browser") );
    leftTabWidget->addTab( localBrowserView,
        KIcon("folder.png"),
        i18n("Local Browser") );

    mainHorizSplitter = new QSplitter( Qt::Horizontal, this );
    mainHorizSplitter->addWidget( leftTabWidget );
    mainHorizSplitter->addWidget( docTabWidget );

    setCentralWidget( mainHorizSplitter );
}

void MainWindow::setupActions()
{
    newDocumentAction = new KAction( i18n("New"), this );
    newDocumentAction->setIcon( KIcon("document-new.png") );

    newConnectionAction = new KAction( i18n("New Connection"), this );
    newConnectionAction->setIcon( KIcon("network-connect.png") );
    connect( newConnectionAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewConnection()) );

    openAction = new KAction( i18n("Open"), this );
    openAction->setIcon( KIcon("document-open.png") );

    settingsAction = new KAction( i18n("Configure Kobby"), this );
    connect( settingsAction, SIGNAL(triggered(bool)),
        this, SLOT(slotShowSettingsDialog()) );

    actionCollection()->addAction( "new_document", newDocumentAction );
    actionCollection()->addAction( "new_connection", newConnectionAction );
    actionCollection()->addAction( "open_local", openAction );
    actionCollection()->addAction( "settings_kobby", settingsAction );
}

void MainWindow::slotNewConnection()
{
    CreateConnectionDialog *dialog = new CreateConnectionDialog( this );
    connect( dialog, SIGNAL(createConnection(const QString&, unsigned int)),
        this, SLOT(slotCreateConnection(const QString&, unsigned int)) );
    dialog->setVisible( true );
}

void MainWindow::slotCreateConnection( const QString &hostname,
    unsigned int port )
{
    Connection *conn = new Connection( hostname, port, this );
    connect( conn, SIGNAL(connected(Connection*)),
        this, SLOT(slotConnectionConnected(Connection*)) );
    connect( conn, SIGNAL(error(Connection*, QString)),
        this, SLOT(slotConnectionError(Connection*, QString)) );
    conn->open();
}

void MainWindow::slotConnectionError( Connection *conn,
    QString errMsg )
{
    QString str = i18n("Error creating conneciton: ");
    str += errMsg;
    KMessageBox::error( this, str );
    slotNewConnection();
}

void MainWindow::slotShowSettingsDialog()
{
    if( KConfigDialog::showDialog("Kobby Settings") )
        return;

    SettingsDialog *dialog = new SettingsDialog( this );
    dialog->show();
}

void MainWindow::slotConnectionConnected( Connection *conn )
{
    browserModel->addConnection( *conn->xmppConnection(), conn->name() );
}

void MainWindow::restoreSettings()
{
    QList<int> sizes;
    sizes = KobbySettings::mainWindowGeometry();
    if( sizes.size() == 4 )
        setGeometry( sizes[0], sizes[1], sizes[2], sizes[3] );
    sizes = KobbySettings::mainWindowHorizSplitterSizes();
    if( sizes.size() )
        mainHorizSplitter->setSizes( sizes );
    else
    {
        sizes.empty();
        sizes << 1 << 10;
        mainHorizSplitter->setSizes( sizes );
    }
}

void MainWindow::saveSettings()
{
    QList<int> sizes;

    sizes << x() << y() << width() << height();
    KobbySettings::setMainWindowGeometry( sizes );
    KobbySettings::setMainWindowHorizSplitterSizes( mainHorizSplitter->sizes() );
    KobbySettings::self()->writeConfig();
}

}
