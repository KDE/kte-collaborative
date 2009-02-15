#ifndef KOBBY_LOCALBROWSERVIEW_H
#define KOBBY_LOCALBROWSERVIEW_H

#include <QWidget>

class KDirOperator;
class KToolBar;

namespace Kobby
{

class LocalBrowserView
    : public QWidget
{

    public:
        LocalBrowserView( QWidget *parent = 0 );
    
    private:
        void setupUi();

        KDirOperator *dirOperator;
        KToolBar *navToolBar;

};

}

#endif

