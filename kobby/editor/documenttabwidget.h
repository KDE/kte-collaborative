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
    Q_OBJECT

    public:
        DocumentTabWidget( QWidget *parent = 0 );
        ~DocumentTabWidget();

        void addDocument( KTextEditor::Document &document );

    Q_SIGNALS:
        void closingDocument( KTextEditor::Document *document );

    private Q_SLOTS:
        void closeCurrentTab();

    private:
        KTextEditor::Document *documentAt( int index );

        QToolButton *closeButton;

};

}

#endif
