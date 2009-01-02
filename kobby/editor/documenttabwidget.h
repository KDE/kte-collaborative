#ifndef KOBBY_DOCUMENTTABWIDGET_H
#define KOBBY_DOCUMENTTABWIDGET_H

#include <QTabWidget>

class QToolButton;

namespace KTextEditor
{
    class Document;
}

namespace Kobby
{

class DocumentTabWidget
    : public QTabWidget
{

    public:
        DocumentTabWidget( QWidget *parent = 0 );
        ~DocumentTabWidget();

        void addDocument( KTextEditor::Document &document );

    private:
        QToolButton *closeButton;

};

}

#endif
