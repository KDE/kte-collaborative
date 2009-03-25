#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <libqinfinity/browsermodel.h>
#include <libqinfinity/sessionproxy.h>

#include <KParts/MainWindow>

#include <QPointer>
#include <QMap>

class QLabel;
class QSplitter;
class KTabWidget;
class KUrl;

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
        void slotNewConnection();
        void slotCreateConnection( const QString &hostname,
            unsigned int port );
        void slotShowSettingsDialog();
        void slotConnectionConnected( Connection *conn );
        void slotConnectionError( Connection *conn, QString );
        void slotOpenUrl( const KUrl &url );
        void slotOpenRemote( const QModelIndex &index );
        void slotSessionSubscribed( const QInfinity::BrowserIter &node,
            QPointer<QInfinity::SessionProxy> sessionProxy );
    
    private:
        void setupUi();
        void setupActions();
        void restoreSettings();
        void saveSettings();
        void joinNote( const QInfinity::BrowserIter &noteItr );

        // Libqinfinity
        QInfinity::BrowserModel *browserModel;
        QInfinity::DefaultTextPlugin *textPlugin;

        KTextEditor::Editor *editor;

        // Ui
        QSplitter *mainHorizSplitter;
        KTabWidget *leftTabWidget;
        RemoteBrowserView *remoteBrowserView;
        LocalBrowserView *localBrowserView;
        DocumentTabWidget *docTabWidget;
        QLabel *statusLabel;

        // Actions
        KAction *newDocumentAction;
        KAction *newConnectionAction;
        KAction *openAction;
        KAction *settingsAction;

};

}

#endif
