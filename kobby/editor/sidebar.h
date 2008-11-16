#ifndef KOBBY_SIDEBAR_H
#define KOBBY_SIDEBAR_H

#include <QTabWidget>

class QTreeView;

namespace Kobby
{

class ConnectionManagerWidget;

class Sidebar
    : public QTabWidget
{

    public:
        Sidebar( QWidget *parent = 0 );

    private:
        QTreeView *m_treeView;
        ConnectionManagerWidget *m_connectionManager;
};

}

#endif

