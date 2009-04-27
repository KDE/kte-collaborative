#ifndef KOBBY_DOCUMENTMANAGER_H
#define KOBBY_DOCUMENTMANAGER_H

#include <QObject>
#include <QPointer>
#include <QHash>

typedef struct _GError GError;

class QModelIndex;
class KUrl;

namespace QInfinity
{
    class BrowserModel;
    class BrowserIter;
    class Browser;
    class SessionProxy;
    class Session;
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
 * The DocumentBuilder handles asynchronous creation of
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
        /**
         * @brief A new document has been created.
         */
        void documentCreated( Document &document );

    public Q_SLOTS:
        /**
         * @brief Open a blank local document.
         */
        void openBlank();

        /**
         * @brief Open Infinote document represented by index.
         */
        void openInfDocmuent( const QModelIndex &index );

        /**
         * @brief Open document from url
         */
        void openUrl( const KUrl &url );

    private Q_SLOTS:
        void sessionSubscribed( const QInfinity::BrowserIter &iter,
            QPointer<QInfinity::SessionProxy> sessProxy );
        void slotSessionSynchronized();
        void slotSessionSynchronizationFailed( GError *error );
        void slotBrowserAdded( QInfinity::Browser &browser );
    
    private:
        void sessionSynchronized( QInfinity::Session *session );

        KTextEditor::Editor *editor;
        QInfinity::BrowserModel *m_browserModel;
        QHash<QInfinity::Session*, QInfinity::SessionProxy*> sessionToProxy;

};

}

#endif

