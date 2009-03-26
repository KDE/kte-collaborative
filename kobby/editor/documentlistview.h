#ifndef KOBBY_DOCUMENT_LIST_VIEW_H
#define KOBBY_DOCUMENT_LIST_VIEW_H

#include <QWidget>

class KAction;
class QListView;
class QContextMenuEvent;
class KMenu;

namespace Kobby
{

class DocumentModel;

class DocumentListView
    : public QWidget
{

    public:
        DocumentListView( DocumentModel &model,
            QWidget *parent = 0 );

    protected:
        void contextMenuEvent( QContextMenuEvent *e );

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

