#ifndef KOBBY_DOCUMENTTABWIDGET_H
#define KOBBY_DOCUMENTTABWIDGET_H

#include <KTabWidget>
#include <QMap>

namespace KTextEditor
{
    class Document;
    class View;
}

namespace Kobby
{

/**
 * @brief Tab widget containing active document views, and map from documents to their view.
 */
class DocumentTabWidget
    : public KTabWidget
{
    Q_OBJECT

    public:
        DocumentTabWidget( QWidget *parent = 0 );
        ~DocumentTabWidget();

        void addDocument( KTextEditor::Document &document );
        void removeDocument( KTextEditor::Document &document );
        KTextEditor::Document *documentAt( int index );
        KTextEditor::View *documentView( KTextEditor::Document &document );

    Q_SIGNALS:
        void documentClose( KTextEditor::Document *document );

    private Q_SLOTS:
        void closeCurrentTab();

    private:
        QMap<KTextEditor::Document*, KTextEditor::View*> documentViewMap;

};

}

#endif
