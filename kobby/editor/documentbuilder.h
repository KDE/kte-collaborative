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
    class BrowserIter;
    class Browser;
    class SessionProxy;
}

namespace KTextEditor
{
    class Editor;
    class Document;
}

namespace Kobby
{

class Document;
class KDocumentTextBuffer;

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

        /**
         * @brief Create a new document.
         *
         * Returns created document, and emits
         * documentCreated signal.
         */
        KDocumentTextBuffer *createKDocumentTextBuffer( const QString &encoding );

    Q_SIGNALS:
        void documentCreated( Document &document );

    public Q_SLOTS:
        void openBlank();
        void openInfDocmuent( const QModelIndex &index );
        void openUrl( const KUrl &url );

    private Q_SLOTS:
        void sessionSubscribed( const QInfinity::BrowserIter &iter,
            QPointer<QInfinity::SessionProxy> sessProxy );
        void slotBrowserAdded( QInfinity::Browser &browser );
    
    private:
        KTextEditor::Editor *editor;
        QInfinity::BrowserModel *m_browserModel;

};

}

#endif

