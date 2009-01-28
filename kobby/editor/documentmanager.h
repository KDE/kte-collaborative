#ifndef KOBBY_DOCUMENTMANAGER_H
#define KOBBY_DOCUMENTMANAGER_H

#include <libqinfinitymm/session.h>

#include <QObject>
#include <QList>
#include <QMap>

namespace QInfinity
{
    class BrowserModel;
    class BrowserNoteItem;
}

namespace KTextEditor
{
    class Editor;
    class Document;
}

namespace Kobby
{

class CollabDocument;

/**
 * @brief Handles the creation and storage of CollabDocument's for each Session.
 */
class DocumentManager
    : public QObject
{
    Q_OBJECT

    public:
        DocumentManager( KTextEditor::Editor &editor,
            QInfinity::BrowserModel &browserModel );
        ~DocumentManager();
    
    Q_SIGNALS:
        /**
         * @brief A document has been created and is loading.
         *
         * Make sure document has completed loading before attempting
         * to access the underlying KTextEditor::Document.
         */
        void documentLoading( CollabDocument &document);

    private Q_SLOTS:
        void slotSessionSubscribed( QInfinity::BrowserNoteItem &note );
    
    private:
        void setupSignals();
        void insertSession( QInfinity::Session &session );

        KTextEditor::Editor *editor;
        QInfinity::BrowserModel *m_browserModel;
        QList<CollabDocument*> m_collabDocuments;
        QMap<KTextEditor::Document*, CollabDocument*> documentCollabDocumentMap;

};

}

#endif

