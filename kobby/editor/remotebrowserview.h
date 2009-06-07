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

#ifndef KOBBY_BROWSERVIEW_H
#define KOBBY_BROWSERVIEW_H

#include <QWidget>
#include <QModelIndexList>

#include <DNSSD/RemoteService>
#include <DNSSD/ServiceBrowser>

class QStackedLayout;
class QTreeView;
class QModelIndex;
class QItemSelection;
class QContextMenuEvent;

class KAction;
class KToolBar;
class KMenu;

namespace QInfinity
{
    class NotePlugin;
    class BrowserModel;
    class XmlConnection;
}

namespace Kobby
{

/**
 * @brief Widget containing toolbar and tree view of a QInfinity::BrowserModel.
 */
class RemoteBrowserView
    : public QWidget
{
    Q_OBJECT

    public:
        RemoteBrowserView( QInfinity::NotePlugin &plugin,
            QInfinity::BrowserModel &model,
            QWidget *parent = 0 );

    Q_SIGNALS:
        void createConnection();
        void openItem( QModelIndex item );
        void deleteItem( QModelIndex item );

    protected:
        void contextMenuEvent( QContextMenuEvent *e );
    
    private Q_SLOTS:
        void slotNewConnection();
        void slotNewDocument();
        void slotNewFolder();
        void slotOpen();
        void slotOpen( const QModelIndex &index );
        void slotDelete();
        void slotSelectionChanged( const QItemSelection &selected,
            const QItemSelection &deselected );
    private:
        void setupActions();
        void setupToolbar();
        bool canCreateDocument( QModelIndexList selected );
        bool canCreateFolder( QModelIndexList selected );
        bool canOpenItem( QModelIndexList selected );
        bool canDeleteItem( QModelIndexList seletected );
        QList<QModelIndex> getSelection();

        QTreeView *m_treeView;
        QInfinity::NotePlugin *m_plugin;
        QInfinity::BrowserModel *browserModel;

        KToolBar *toolBar;
        KAction *createConnectionAction;
        KAction *createDocumentAction;
        KAction *createFolderAction;
        KAction *openAction;
        KAction *deleteAction;
        KMenu *contextMenu;
        
        QHash<QString, QString> m_protocols;


};

/**
 * @brief Proxy of RemoteBrowserView which displays help if there are no connections.
 */
class RemoteBrowserProxy
    : public QWidget
{
    Q_OBJECT

    public:
        RemoteBrowserProxy( QInfinity::NotePlugin &plugin,
            QInfinity::BrowserModel &model,
            QWidget *parent = 0 );

        RemoteBrowserView &remoteView() const;
        
        void startZeroconfScan();

    Q_SIGNALS:
        void createConnection();
        void createConnection( const QString, unsigned int );
        void deleteConnection( const QString, unsigned int );

    private Q_SLOTS:
        void connectionAdded( QInfinity::XmlConnection &connection );
        void connectionRemoved( QInfinity::XmlConnection &connection );
        void resolveNameAdd( DNSSD::RemoteService::Ptr pointer );
        void resolveNameDelete( DNSSD::RemoteService::Ptr pointer );
    

    private:
        QStackedLayout *stackedLayout;
        RemoteBrowserView *m_remoteView;
        QWidget *noActiveWidget;
        DNSSD::ServiceBrowser* z_browser;

};

}

#endif

