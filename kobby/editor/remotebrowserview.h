#ifndef KOBBY_BROWSERVIEW_H
#define KOBBY_BROWSERVIEW_H

#include <QWidget>
#include <QModelIndexList>

class QTreeView;
class QModelIndex;
class QItemSelection;
class QContextMenuEvent;

class KAction;
class KToolBar;
class KMenu;

namespace QInfinity
{
    class BrowserModel;
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
        RemoteBrowserView( QInfinity::BrowserModel &model,
            QWidget *parent = 0 );

    Q_SIGNALS:
        void createConnection();
        void createDocument( QModelIndex parent );
        void createFolder( QModelIndex parent );
        void openItem( QModelIndex item );
        void deleteItem( QModelIndex item );

    protected:
        void contextMenuEvent( QContextMenuEvent *e );
    
    private Q_SLOTS:
        void slotNewConnection();
        void slotNewDocument();
        void slotNewFolder();
        void slotOpen();
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
        QItemSelection getSelection();

        QTreeView *m_treeView;
        QInfinity::BrowserModel *browserModel;

        KToolBar *toolBar;
        KAction *createConnectionAction;
        KAction *createDocumentAction;
        KAction *createFolderAction;
        KAction *openAction;
        KAction *deleteAction;
        KMenu *contextMenu;
};

}

#endif

