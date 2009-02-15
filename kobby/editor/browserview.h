#ifndef KOBBY_BROWSERVIEW_H
#define KOBBY_BROWSERVIEW_H

#include <libqinfinity/browsermodel.h>

#include <QWidget>

class QTreeView;
class KAction;

namespace Kobby
{

class BrowserView
    : public QWidget
{

    public:
        BrowserView( QInfinity::BrowserModel &model,
            QWidget *parent = 0 );
    
    private:
        QTreeView *m_treeView;
        KAction *createFolderAction;
        KAction *createDocumentAction;
        KAction *deleteAction;
};

}

#endif

