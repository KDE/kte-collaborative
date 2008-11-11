#ifndef KOBBY_SIDEBAR_H
#define KOBBY_SIDEBAR_H

#include <QTabWidget>

class QTreeView;

namespace QInfinity
{
    class ConnectionListWidget;
}

namespace Kobby
{

class Sidebar
    : public QTabWidget
{

    public:
        Sidebar( QWidget *parent = 0 );

    private:
        QTreeView *m_treeView;
        QInfinity::ConnectionListWidget *m_connectionList;
};

}

#endif

