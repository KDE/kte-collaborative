#ifndef KOBBY_FILEBROWSERWIDGET_H
#define KOBBY_FILEBROWSERWIDGET_H

#include <QWidget>

class QTreeView;
class QModelIndex;

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

    private:
        void setupUi();
        void setupActions();

        QTreeView *m_treeView;
        QInfinity::BrowserModel *fileModel;

};

}

#endif

