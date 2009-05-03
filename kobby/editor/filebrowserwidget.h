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

#ifndef KOBBY_FILEBROWSERWIDGET_H
#define KOBBY_FILEBROWSERWIDGET_H

#include <QWidget>

class KAction;

class QTreeView;
class QModelIndex;
class QContextMenuEvent;
class QItemSelection;
class QString;
class QStandardItem;

namespace QInfinity
{
    class BrowserModel;
    class BrowserItem;
    class BrowserFolderItem;
}

namespace Kobby
{

class BrowserModel;

/**
 * @brief Widget displaying BrowserModel as tree and context menu.
 */
class FileBrowserWidget
    : public QWidget
{
    Q_OBJECT

    public:
        FileBrowserWidget( BrowserModel &model,
            QWidget *parent = 0 );

    Q_SIGNALS:
        void itemOpened( QInfinity::BrowserItem &item );

    public Q_SLOTS:
        void createFolder( QInfinity::BrowserFolderItem &parent,
            QString name );
        void createNote( QInfinity::BrowserFolderItem &parent,
            Infinity::ClientNotePlugin &notePlugin,
            QString name );

    protected:
        void contextMenuEvent( QContextMenuEvent *e );

    private Q_SLOTS:
        void slotSelectionChanged( const QItemSelection &selected,
            const QItemSelection &deselected );
        void slotDeleteSelected();
        void slotCreateFolder();
        void slotCreateNote();
        void slotItemActivated( const QModelIndex& );

    private:
        void setupUi();
        void setupActions();
        bool canHaveChildren( const QModelIndex &index );
        /**
         * Returns pointer to the selected item or 0 if
         * no items selected or multiple items selected.
         */
        QStandardItem *getSingleSelectedItem();

        QTreeView *m_treeView;
        QInfinity::BrowserModel *fileModel;
        KAction *deleteItemAction;
        KAction *createFolderAction;
        KAction *createNoteAction;
        KAction *joinNoteAction;

};

}

#endif

