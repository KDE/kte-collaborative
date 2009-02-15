#include "mainwindow.h"
#include "kobbysettings.h"
#include "settingsdialog.h"
#include "browserview.h"
#include "documenttabwidget.h"

#include <libqinfinity/filemodel.h>

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

    fileModel = new QInfinity::FileModel( this );
    docTabWidget = new DocumentTabWidget( this );

    setXMLFile( "kobbyui.rc" );

    KTextEditor::Document *doc = editor->createDocument( this );
    docTabWidget->addDocument( *doc );

    guiFactory()->addClient( docTabWidget->documentView( *doc ) );
    createShellGUI( true );
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

    browserView = new BrowserView( *fileModel, this );

    leftTabWidget = new KTabWidget( this );
    leftTabWidget->setTabPosition( QTabWidget::West );
    leftTabWidget->addTab( browserView,
        KIcon("folder.png"),
        i18n("Files") );

    mainHorizSplitter = new QSplitter( Qt::Horizontal, this );
    mainHorizSplitter->addWidget( leftTabWidget );
    mainHorizSplitter->addWidget( docTabWidget );

    setCentralWidget( mainHorizSplitter );
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
