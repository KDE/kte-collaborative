#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <KSharedConfig>
#include <KParts/MainWindow>

#include <QtGui/QKeyEvent>
#include <QMap>

#include <glibmm/refptr.h>

class KConfigGroup;
class QSplitter;
class QTabWidget;

namespace KTextEditor
{
    class Editor;
    class Document;
    class View;
}

namespace Infinity
{
    class ClientSessionProxy;
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
        void slotSessionSubscribed( QInfinity::BrowserNoteItem &node,
            Glib::RefPtr<Infinity::ClientSessionProxy> sessionProxy );
        void slotDocumentTabChanged( int index );
        void slotDocumentClose( KTextEditor::Document* );
    
    private:
        void init();
        void setupUi();
        void setupActions();
        void loadConfig();
        void saveConfig();
        
        QInfinity::InfinoteManager *infinoteManager;
        ConnectionManagerWidget *connectionManager;
        BrowserModel *browserModel;
        FileBrowserWidget *fileBrowser;
        
        QSplitter *mainSplitter;
        Sidebar *m_sidebar;
        DocumentTabWidget *documentTab;

        KTextEditor::Editor *editor;
        KTextEditor::View *curr_view;
        KTextEditor::Document *curr_document;
        CollabDocument *curr_collabDocument;
        QList<CollabDocument*> collabDocuments;
        QMap<KTextEditor::Document*, CollabDocument*> collabDocumentMap;
        
        KAction *newDocumentAction;
        KAction *newConnectionAction;
        KAction *controlAction;
        KAction *settingsAction;

        KSharedConfigPtr configptr;
        KConfigGroup *configGeneralGroup;

};

}

#endif
