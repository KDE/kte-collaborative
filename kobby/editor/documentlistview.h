#ifndef KOBBY_DOCUMENT_LIST_VIEW_H
#define KOBBY_DOCUMENT_LIST_VIEW_H

#include <QWidget>

class KAction;
class KMenu;
class QListView;
class QContextMenuEvent;

namespace Kobby
{

class DocumentModel;

class DocumentListView
    : public QWidget
{
    Q_OBJECT;

    public:
        DocumentListView( DocumentModel &model,
            QWidget *parent = 0 );

    protected:
        void contextMenuEvent( QContextMenuEvent *e );

    private Q_SLOTS:
        void closeSelected();

    private:
        void setupUi();
        void setupActions();

        KAction *closeAction;

        QListView *listView;
        DocumentModel *docModel;
        KMenu *contextMenu;

};

}

#endif

