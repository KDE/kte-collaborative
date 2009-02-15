#ifndef KOBBY_BROWSERVIEW_H
#define KOBBY_BROWSERVIEW_H

#include <QWidget>

class QTreeView;
class KAction;
class KToolBar;

namespace QInfinity
{
    class BrowserModel;
}

namespace Kobby
{

class RemoteBrowserView
    : public QWidget
{
    Q_OBJECT

    public:
        RemoteBrowserView( QInfinity::BrowserModel &model,
            QWidget *parent = 0 );

    Q_SIGNALS:
        void createConnection();
    
    private Q_SLOTS:
        void slotNewConnection();
        void slotNewDocument();
        void slotNewFolder();
        void slotOpen();
        void slotDelete();
    
    private:
        void setupActions();
        void setupToolbar();

        QTreeView *m_treeView;
        QInfinity::BrowserModel *browserModel;

        KToolBar *toolBar;
        KAction *createConnectionAction;
        KAction *createDocumentAction;
        KAction *createFolderAction;
        KAction *openAction;
        KAction *deleteAction;
};

}

#endif

