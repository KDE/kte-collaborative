#ifndef KOBBY_LOCALBROWSERVIEW_H
#define KOBBY_LOCALBROWSERVIEW_H

#include <QWidget>

class KDirOperator;
class KToolBar;
class KFileItem;
class KUrl;
class QAction;
class QModelIndex;
class QAbstractItemView;

namespace Kobby
{

class LocalBrowserView
    : public QWidget
{
    Q_OBJECT

    public:
        LocalBrowserView( QWidget *parent = 0 );

        QAction *action( const QString &name );

    Q_SIGNALS:
        void urlSelected( const KUrl &url );

    private Q_SLOTS:
        void slotFileSelected( const QModelIndex& );
        void slotViewChanged( QAbstractItemView *newView );
    
    private:
        void setupUi();

        KDirOperator *dirOperator;
        KToolBar *navToolBar;

};

}

#endif

