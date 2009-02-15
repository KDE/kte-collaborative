#include "mainwindow.h"
#include "kobbysettings.h"
#include "settingsdialog.h"
#include "browserview.h"
#include "documenttabwidget.h"
#include "connection.h"
#include "createconnectiondialog.h"
#include "itemfactory.h"

#include <libqinfinity/browseritemfactory.h>

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

#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <KTextEditor/EditorChooser>
#include <KTextEditor/Document>

#include <QLabel>
#include <QSplitter>
#include <QTreeView>
#include <QTabWidget>
#include <QStatusBar>

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

    browserView = new BrowserView( *browserModel, this );

    leftTabWidget = new KTabWidget( this );
    leftTabWidget->setTabPosition( QTabWidget::West );
    leftTabWidget->addTab( browserView,
        KIcon("document-open-remote.png"),
        i18n("Remote Browser") );

    mainHorizSplitter = new QSplitter( Qt::Horizontal, this );
    mainHorizSplitter->addWidget( leftTabWidget );
    mainHorizSplitter->addWidget( docTabWidget );

    setCentralWidget( mainHorizSplitter );
}

void MainWindow::setupActions()
{
    newDocumentAction = new KAction( i18n("New Document"), this );
    newDocumentAction->setIcon( KIcon("document-new.png") );
    newConnectionAction = new KAction( i18n("New Connection"), this );
    newConnectionAction->setIcon( KIcon("network-connect.png") );
    settingsAction = new KAction( i18n("Configure Kobby"), this );
    connect( settingsAction, SIGNAL(triggered(bool)),
        this, SLOT(showSettingsDialog()) );

    actionCollection()->addAction( "new_document", newDocumentAction );
    actionCollection()->addAction( "new_connection", newConnectionAction );
    actionCollection()->addAction( "settings_kobby", settingsAction );
}

void MainWindow::newConnection( bool checked )
{
    Q_UNUSED( checked )

    CreateConnectionDialog *dialog = new CreateConnectionDialog( this );
    connect( dialog, SIGNAL(createConnection(const QString&, unsigned int)),
        this, SLOT(createConnection(const QString&, unsigned int)) );
    dialog->setVisible( true );
}

void MainWindow::createConnection( const QString &hostname,
    unsigned int port )
{
    Connection *conn = new Connection( hostname, port, this );
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

void MainWindow::showSettingsDialog()
{
    if( KConfigDialog::showDialog("Kobby Settings") )
        return;

    SettingsDialog *dialog = new SettingsDialog( this );
    dialog->show();
}

}
