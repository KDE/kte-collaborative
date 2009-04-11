#include "mainwindow.h"
#include "kobbysettings.h"
#include "settingsdialog.h"
#include "remotebrowserview.h"
#include "localbrowserview.h"
#include "documentlistview.h"
#include "documenttabwidget.h"
#include "documentmodel.h"
#include "documentbuilder.h"
#include "document.h"
#include "connection.h"
#include "createconnectiondialog.h"
#include "itemfactory.h"

#include <libqinfinity/defaulttextplugin.h>
#include <libqinfinity/browser.h>
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

#include <KDebug>

#include "mainwindow.moc"

namespace Kobby
{

MainWindow::MainWindow( QWidget *parent )
{
    Q_UNUSED( parent );

    editor = KTextEditor::EditorChooser::editor();
    if( !editor )
    {
        KMessageBox::error(this, i18n("A KDE text-editor component could not be found;\n"
            "please check your KDE installation."));
        kapp->exit(1);
    }

    // Setup the QInfinity BrowserModel
    browserModel = new QInfinity::BrowserModel( this );
    browserModel->setItemFactory( new ItemFactory( this ) );
    textPlugin = new QInfinity::DefaultTextPlugin( this );
    browserModel->addPlugin( *textPlugin );

    // Setup Document Management
    docTabWidget = new DocumentTabWidget( this );
    docModel = new DocumentModel( this );
    docBuilder = new DocumentBuilder( *editor, *browserModel, this );
    connect( docBuilder, SIGNAL(documentCreated(Document&)),
        docModel, SLOT(insertDocument(Document&)) );
    connect( docModel, SIGNAL(documentAdded(Document&)),
        docTabWidget, SLOT(addDocument(Document&)) );
    connect( docModel, SIGNAL(documentAboutToBeRemoved(Document&)),
        docTabWidget, SLOT(removeDocument(Document&)) );
    connect( docTabWidget, SIGNAL(viewRemoved(KTextEditor::View&)),
        this, SLOT(slotViewRemoved(KTextEditor::View&)) );

    setXMLFile( "kobby/kobbyui.rc" );
    setupUi();
    setupActions();
    createShellGUI( true );

    docBuilder->openBlank();
    mergeView( docTabWidget->activeView() );

    restoreSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUi()
{
    setWindowIcon( KIcon( "meeting-attending.png" ) );

    statusLabel = new QLabel( this );
    QStatusBar *statusBar = new QStatusBar( this );
    statusBar->addWidget( statusLabel );
    setStatusBar( statusBar );

    // Setup RemoteBrowserView
    remoteBrowserView = new RemoteBrowserView( *textPlugin,
        *browserModel, this );
    connect( remoteBrowserView, SIGNAL(createConnection()),
        this, SLOT(slotNewConnection()) );
    connect( remoteBrowserView, SIGNAL(openItem(const QModelIndex&)),
        docBuilder, SLOT(openInfDocmuent(const QModelIndex&)) );

    // Setup LocalBrowserView
    localBrowserView = new LocalBrowserView( this );
    connect( localBrowserView, SIGNAL(urlSelected(const KUrl&)),
        docBuilder, SLOT(openUrl(const KUrl&)) );

    documentListView = new DocumentListView( *docModel, this );

    // Setup Left Tab Bar
    leftTabWidget = new KTabWidget( this );
    leftTabWidget->setTabPosition( QTabWidget::West );
    leftTabWidget->addTab( documentListView,
        KIcon("document-preview.png"),
        i18n("Documents") );
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
    newConnectionAction = new KAction( i18n("New Connection"), this );
    openAction = new KAction( i18n("Open"), this );
    settingsAction = new KAction( i18n("Configure Kobby"), this );

    newDocumentAction->setIcon( KIcon("document-new.png") );
    newConnectionAction->setIcon( KIcon("network-connect.png") );
    openAction->setIcon( KIcon("document-open.png") );

    connect( newDocumentAction, SIGNAL(triggered(bool)),
        docBuilder, SLOT(openBlank()) );
    connect( newConnectionAction, SIGNAL(triggered(bool)),
        this, SLOT(slotNewConnection()) );
    connect( settingsAction, SIGNAL(triggered(bool)),
        this, SLOT(slotShowSettingsDialog()) );

    KStandardAction::quit(kapp, SLOT(quit()),
        actionCollection());

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
    Q_UNUSED(conn);
    QString str = i18n("Error with connecton: ");
    str += errMsg;
    KMessageBox::error( this, str );
    slotNewConnection();
}

void MainWindow::slotViewDestroyed( QObject *obj )
{
    slotViewRemoved( *dynamic_cast<KTextEditor::View*>(obj) );
}

void MainWindow::slotViewRemoved( KTextEditor::View &view )
{
    KTextEditor::View *tmpView;
    kDebug() << "Removing view " << docTabWidget->count();
    if( merged_view == &view )
    {
        guiFactory()->removeClient( &view );
        tmpView = docTabWidget->activeView();
        if( !tmpView || (tmpView == merged_view && docTabWidget->count() <= 1))
        {
            if( true )
            {
                docBuilder->openBlank();
                tmpView = docTabWidget->activeView();
                if( !tmpView )
                {
                    kDebug() << "Unable to create new view to merge.";
                    return;
                }
            }
        }
        mergeView( tmpView );
    }
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
    QInfinity::ConnectionItem *item;
    // We are using our subclassed QInfinity ConnectionItem
    item = browserModel->addConnection( *conn->xmppConnection(), conn->name() );
    dynamic_cast<Kobby::ConnectionItem*>(item)->setConnection( conn );
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

    if( KobbySettings::hostName().isEmpty() )
        KobbySettings::setHostName( QHostInfo::localHostName() );
}

void MainWindow::saveSettings()
{
    QList<int> sizes;

    sizes << x() << y() << width() << height();
    KobbySettings::setMainWindowGeometry( sizes );
    KobbySettings::setMainWindowHorizSplitterSizes( mainHorizSplitter->sizes() );
    KobbySettings::self()->writeConfig();
}

void MainWindow::mergeView( KTextEditor::View *view )
{
    merged_view = view;
    connect( view, SIGNAL(destroyed(QObject*)),
        this, SLOT(slotViewDestroyed(QObject*)) );
    guiFactory()->addClient( view );
}

}
