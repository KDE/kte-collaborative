/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KOBBY_EDITOR_MAINWINDOW_H
#define KOBBY_EDITOR_MAINWINDOW_H

#include <KParts/MainWindow>

#include <QPointer>

class KUrl;
class KTabWidget;

class QLabel;
class QDockWidget;

namespace QInfinity
{
    class ConnectionItem;
    class BrowserModel;
    class SessionProxy;
}

namespace KTextEditor
{
    class Editor;
    class Document;
    class View;
}

namespace Kobby
{

class RemoteBrowserProxy;
class LocalBrowserView;
class DocumentUsersBrowser;
class DocumentListView;
class DocumentTabWidget;
class Connection;
class Document;
class DocumentModel;
class DocumentBuilder;
class NotePlugin;

class MainWindow
    : public KParts::MainWindow
{
    Q_OBJECT;
    
    public:
        MainWindow( QWidget *parent = 0 );
        ~MainWindow();

    protected:
        bool queryClose();

    private Q_SLOTS:
        void slotNewConnection();
        void slotCreateConnection( const QString &hostname,
            unsigned int port );
        void slotOpenFile();
        void slotShowSettingsDialog();
        void slotConnectionConnected( Connection *conn );
        void slotConnectionError( Connection *conn, QString );
        void slotCloseActive();
        void slotTextViewActivated( KTextEditor::View *view );
        void slotDocumentFatalError( Document *doc, QString message );
        void slotQuit();
    
    private:
        void setupUi();
        void setupActions();
        void restoreSettings();
        void saveSettings();
        Document *activeDocument();
        bool needsSetupDialog();
        bool tryQuit();

        // Libqinfinity
        QInfinity::BrowserModel *browserModel;
        NotePlugin *textPlugin;

        KTextEditor::Editor *editor;
        DocumentModel *docModel;
        DocumentBuilder *docBuilder;

        // Ui
        QDockWidget *fileBrowserDockWidget;
        QDockWidget *doclistDockWidget;
        QDockWidget *userlistDockWidget;
        KTabWidget *fileBrowserTabWidget;
        DocumentUsersBrowser *usersBrowser;
        RemoteBrowserProxy *remoteBrowserView;
        LocalBrowserView *localBrowserView;
        DocumentListView *documentListView;
        DocumentTabWidget *docTabWidget;
        QLabel *statusLabel;
        QPointer<KTextEditor::View> mergedTextView;

        // Actions
        KAction *newConnectionAction;
        KAction *settingsAction;
        QAction *edit_undo;
        QAction *edit_redo;

};

}

#endif
