#ifndef KOBBY_FILEBROWSERWIDGET_H
#define KOBBY_FILEBROWSERWIDGET_H

#include <QWidget>

class KAction;

class QTreeView;
class QModelIndex;
class QContextMenuEvent;
class QItemSelection;

namespace QInfinity
{
    class BrowserModel;
    class BrowserItem;
}

namespace Kobby
{

class FileBrowserWidget
    : public QWidget
{
    Q_OBJECT

    public:
        FileBrowserWidget( QWidget *parent = 0 );

    Q_SIGNALS:
        void itemOpened( QInfinity::BrowserItem &item );

    protected:
        void contextMenuEvent( QContextMenuEvent *e );

    private Q_SLOTS:
        void slotSelectionChanged( const QItemSelection &selected,
            const QItemSelection &deselected );
        void slotDeleteSelected();
        void slotCreateFolder();

    private:
        void setupUi();
        void setupActions();
        bool canHaveChildren( const QModelIndex &index );

        QTreeView *m_treeView;
        QInfinity::BrowserModel *fileModel;
        KAction *deleteItemAction;
        KAction *createFolderAction;

};

}

#endif

