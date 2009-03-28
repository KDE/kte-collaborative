#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <libqinfinity/browsermodel.h>
#include <libqinfinity/sessionproxy.h>

#include <KParts/MainWindow>

#include <QPointer>
#include <QMap>

class KTabWidget;
class KUrl;
class QLabel;
class QSplitter;

namespace QInfinity
{
    class ConnectionItem;
    class DefaultTextPlugin;
}

namespace KTextEditor
{
    class Editor;
    class Document;
    class View;
}

namespace Kobby
{

class RemoteBrowserView;
class LocalBrowserView;
class DocumentListView;
class DocumentTabWidget;
class Connection;
class Document;
class DocumentModel;
class DocumentBuilder;

class MainWindow
    : public KParts::MainWindow
{
    Q_OBJECT;
    
    public:
        MainWindow( QWidget *parent = 0 );
        ~MainWindow();

    private Q_SLOTS:
        void slotNewConnection();
        void slotCreateConnection( const QString &hostname,
            unsigned int port );
        void slotShowSettingsDialog();
        void slotConnectionConnected( Connection *conn );
        void slotConnectionError( Connection *conn, QString );
        void slotViewDestroyed( QObject *obj );
        void slotViewRemoved( KTextEditor::View &view );
    
    private:
        void setupUi();
        void setupActions();
        void restoreSettings();
        void saveSettings();
        void mergeView( KTextEditor::View *view );

        // Libqinfinity
        QInfinity::BrowserModel *browserModel;
        QInfinity::DefaultTextPlugin *textPlugin;

        KTextEditor::Editor *editor;
        DocumentModel *docModel;
        DocumentBuilder *docBuilder;

        // Ui
        QSplitter *mainHorizSplitter;
        KTabWidget *leftTabWidget;
        RemoteBrowserView *remoteBrowserView;
        LocalBrowserView *localBrowserView;
        DocumentListView *documentListView;
        DocumentTabWidget *docTabWidget;
        QLabel *statusLabel;
        KTextEditor::View *merged_view;

        // Actions
        KAction *newDocumentAction;
        KAction *newConnectionAction;
        KAction *openAction;
        KAction *settingsAction;

};

}

#endif
