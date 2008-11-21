#ifndef KOBBY_SIDEBAR_H
#define KOBBY_SIDEBAR_H

#include <QTabWidget>

class QTreeView;

namespace Kobby
{

class ConnectionManagerWidget;
class FileBrowserWidget;

class Sidebar
    : public QTabWidget
{

    public:
        Sidebar( QWidget *parent = 0 );

};

}

#endif

