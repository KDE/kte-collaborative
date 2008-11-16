#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <KSharedConfig>
#include <KParts/MainWindow>
#include <QtGui/QKeyEvent>

class KConfigGroup;
class QSplitter;

namespace KTextEditor
{
    class Editor;
    class Document;
    class View;
}

namespace QInfinity
{
    class InfinoteManager;
}

namespace Kobby
{

class Sidebar;

class MainWindow
    : public KParts::MainWindow
{
    Q_OBJECT
    
    public:
        MainWindow( QWidget *parent = 0 );
        ~MainWindow();
    
    private Q_SLOTS:
        void openControlDialog();
        void openSettingsDialog();
        void slotCreateConnection();
        void slotProxy( const QString &, const QString &, unsigned int );
    
    private:
        void init();
        void setupActions();
        void loadConfig();
        void saveConfig();
        
        QInfinity::InfinoteManager *infinoteManager;
        
        QSplitter *mainSplitter;
        Sidebar *m_sidebar;
        KSharedConfigPtr configptr;
        KConfigGroup *configGeneralGroup;
        KTextEditor::Editor *editor;
        KTextEditor::View *curr_view;
        KTextEditor::Document *curr_document;
        
        KAction *newDocumentAction;
        KAction *newConnectionAction;
        KAction *controlAction;
        KAction *settingsAction;
};

}

#endif
