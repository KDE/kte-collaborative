#ifndef KOBBY_DOCUMENTMANAGER_H
#define KOBBY_DOCUMENTMANAGER_H

#include <QObject>
#include <QPointer>
#include <QHash>

class QModelIndex;
class KUrl;

namespace QInfinity
{
    class BrowserModel;
    class Browser;
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
            QInfinity::BrowserModel &browserModel,
            QObject *parent = 0 );
        ~DocumentBuilder();
    
    Q_SIGNALS:
        void documentCreated( Document &document );

    public Q_SLOTS:
        void openBlank();
        void openInfDocmuent( const QModelIndex &index );
        void openUrl( const KUrl &url );
    
    private:
        KTextEditor::Editor *editor;
        QInfinity::BrowserModel *m_browserModel;

};

}

#endif

