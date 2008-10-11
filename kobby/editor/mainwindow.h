#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <KParts/MainWindow>
#include <QtGui/QKeyEvent>

namespace KTextEditor
{
    class Document;
    class View;
}

namespace Kobby
{

class InfinoteManager;

class MainWindow
    : public KParts::MainWindow
{
    Q_OBJECT
    
    public:
        MainWindow( QWidget *parent = 0 );
        ~MainWindow();
    
    private Q_SLOTS:
        void openControlDialog();
    
    private:
        void setupActions();
        
        InfinoteManager *infinoteManager;
        
        KTextEditor::View *curr_view;
        KTextEditor::Document *curr_document;
        KAction *controlAction;
};

}

#endif
