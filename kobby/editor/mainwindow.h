#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <KParts/MainWindow>

#include <QtGui/QKeyEvent>
#include <QMap>

class QSplitter;
class QTabWidget;

namespace KTextEditor
{
    class Editor;
    class Document;
    class View;
}

namespace QInfinity
{
    class InfinoteManager;
    class BrowserItem;
    class BrowserNoteItem;
    class Connection;
    class Document;
}

namespace Kobby
{

class Sidebar;
class ConnectionManagerWidget;
class BrowserModel;
class FileBrowserWidget;
class CollabDocument;
class DocumentTabWidget;
class DocumentManager;

class MainWindow
    : public KParts::MainWindow
{
    Q_OBJECT
    
    public:
        MainWindow( QWidget *parent = 0 );
        ~MainWindow();
    
    private Q_SLOTS:
        void slotCreateConnection();
        void slotOpenItem( QInfinity::BrowserItem &item );
        void showSettingsDialog();
        void loadingDocument( CollabDocument &document );
        void loadedDocument( CollabDocument &document );
    
    private:
        void init();
        void setupUi();
        void setupActions();
        void setupSignals();
        void loadSettings();
        void saveSettings();
        
        QInfinity::InfinoteManager *infinoteManager;
        ConnectionManagerWidget *connectionManager;
        BrowserModel *browserModel;
        FileBrowserWidget *fileBrowser;

        KTextEditor::Editor *editor;
        DocumentTabWidget *documentTab;
        DocumentManager *documentManager;
        
        QSplitter *mainSplitter;
        Sidebar *m_sidebar;
        
        KAction *newDocumentAction;
        KAction *newConnectionAction;
        KAction *controlAction;
        KAction *settingsAction;

};

}

#endif
