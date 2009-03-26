#ifndef KOBBY_DOCUMENTTABWIDGET_H
#define KOBBY_DOCUMENTTABWIDGET_H

#include <KTabWidget>
#include <QHash>

namespace KTextEditor
{
    class Document;
    class View;
}

namespace Kobby
{

class Document;

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

        KTextEditor::View *viewAt( int index );

    public Q_SLOTS:
        /**
         * @brief Create new view of document and add as a new tab.
         */
        void addDocument( Document &doc );

        /**
         * @brief Remove all views of document
         */
        void removeDocument( Document &doc );

    private Q_SLOTS:
        void closeWidget( QWidget *widget );
        void addDocument( KTextEditor::Document &document );
        void removeDocument( KTextEditor::Document &document );

};

}

#endif
