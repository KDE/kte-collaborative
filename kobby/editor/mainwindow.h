#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <KSharedConfig>
#include <KParts/MainWindow>
#include <QtGui/QKeyEvent>

class KConfigGroup;

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
    
    private:
        void init();
        void setupActions();
        
        QInfinity::InfinoteManager *infinoteManager;
        
        KSharedConfigPtr configptr;
        KConfigGroup *configGeneralGroup;
        KTextEditor::Editor *editor;
        KTextEditor::View *curr_view;
        KTextEditor::Document *curr_document;
        KAction *controlAction;
        KAction *settingsAction;
};

}

#endif
