#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <KParts/MainWindow>

class QLabel;
class QSplitter;
class KTabWidget;

namespace QInfinity
{
    class FileModel;
}

namespace KTextEditor
{
    class Editor;
    class Document;
    class View;
}

namespace Kobby
{

class BrowserView;
class DocumentTabWidget;

class MainWindow
    : public KParts::MainWindow
{
    Q_OBJECT
    
    public:
        MainWindow( QWidget *parent = 0 );
        ~MainWindow();
    
    private:
        void setupUi();
        void restoreSettings();
        void saveSettings();
        void showSettingsDialog();

        // Libqinfinity
        QInfinity::FileModel *fileModel;

        KTextEditor::Editor *editor;

        // Ui
        QSplitter *mainHorizSplitter;
        KTabWidget *leftTabWidget;
        BrowserView *browserView;
        DocumentTabWidget *docTabWidget;
        QLabel *statusLabel;

        // Actions
        KAction *newDocumentAction;
        KAction *newConnectionAction;
        KAction *controlAction;
        KAction *settingsAction;

};

}

#endif
