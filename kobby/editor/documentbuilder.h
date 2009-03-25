#ifndef KOBBY_DOCUMENTMANAGER_H
#define KOBBY_DOCUMENTMANAGER_H

#include <QObject>

class QModelIndex;
class KUrl;

namespace QInfinity
{
    class BrowserModel;
}

namespace KTextEditor
{
    class Editor;
    class Document;
}

namespace Kobby
{

class Document;

/**
 * @brief Handles the creation of documents.
 *
 * The DocumentBuilder handles the creation of
 * KTextEditor::Document's from multiple sources.  It
 * also creates and stores wrapper objects for the created
 * doucuments if needed.
 */
class DocumentBuilder
    : public QObject
{
    Q_OBJECT;

    public:
        DocumentBuilder( KTextEditor::Editor &editor,
            QInfinity::BrowserModel &browserModel );
        ~DocumentBuilder();
    
    Q_SIGNALS:
        void documentCreated( Document &document );

    public Q_SLOTS:
        void openInfDocmuent( const QModelIndex &index );
        void openUrl( const KUrl &url );
    
    private:
        void setupSignals();

        KTextEditor::Editor *editor;
        QInfinity::BrowserModel *m_browserModel;

};

}

#endif

