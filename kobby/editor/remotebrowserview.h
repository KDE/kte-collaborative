#ifndef KOBBY_BROWSERVIEW_H
#define KOBBY_BROWSERVIEW_H

#include <libqinfinity/browsermodel.h>

#include <QWidget>

class QTreeView;
class KAction;
class KToolBar;

namespace Kobby
{

class RemoteBrowserView
    : public QWidget
{

    public:
        RemoteBrowserView( QInfinity::BrowserModel &model,
            QWidget *parent = 0 );
    
    private:
        void setupActions();
        void setupToolbar();

        QTreeView *m_treeView;

        KToolBar *toolBar;
        KAction *createDocumentAction;
        KAction *createFolderAction;
        KAction *openAction;
        KAction *deleteAction;
};

}

#endif

