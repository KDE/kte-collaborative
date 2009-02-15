#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <libqinfinity/browsermodel.h>

#include <KParts/MainWindow>

#include <QMap>

class QLabel;
class QSplitter;
class KTabWidget;

namespace QInfinity
{
    class ConnectionItem;
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
class Connection;

class MainWindow
    : public KParts::MainWindow
{
    Q_OBJECT
    
    public:
        MainWindow( QWidget *parent = 0 );
        ~MainWindow();

    private Q_SLOTS:
        void newConnection( bool checked );
        void createConnection( const QString &hostname,
            unsigned int port );
        void showSettingsDialog();
    
    private:
        void setupUi();
        void setupActions();
        void restoreSettings();
        void saveSettings();

        // Libqinfinity
        QInfinity::BrowserModel *browserModel;

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
        KAction *settingsAction;

};

}

#endif
