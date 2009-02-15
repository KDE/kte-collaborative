#ifndef KOBBY_BROWSERVIEW_H
#define KOBBY_BROWSERVIEW_H

#include <libqinfinity/filemodel.h>

#include <QWidget>

class QTreeView;
class KAction;

namespace QInfinity
{
    class FileModel;
}

namespace Kobby
{

class BrowserView
    : public QWidget
{

    public:
        BrowserView( QInfinity::FileModel &model,
            QWidget *parent = 0 );
    
    private:
        QTreeView *m_treeView;
        KAction *createFolderAction;
        KAction *createDocumentAction;
        KAction *deleteAction;
};

}

#endif

