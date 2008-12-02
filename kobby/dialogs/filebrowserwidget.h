#ifndef KOBBY_FILEBROWSERWIDGET_H
#define KOBBY_FILEBROWSERWIDGET_H

#include <QWidget>

class KAction;

class QTreeView;
class QModelIndex;
class QContextMenuEvent;
class QItemSelection;
class QString;

namespace QInfinity
{
    class BrowserModel;
    class BrowserItem;
    class BrowserFolderItem;
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

    public Q_SLOTS:
        void createFolder( QInfinity::BrowserFolderItem &parent,
            QString name );

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

