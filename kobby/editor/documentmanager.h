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

class DocumentManager
    : public QObject
{
    Q_OBJECT

    public:
        DocumentManager( KTextEditor::Editor &editor,
            QInfinity::BrowserModel &browserModel );
        ~DocumentManager();
    
    Q_SIGNALS:
        void documentLoading( CollabDocument &document);

    private Q_SLOTS:
        void slotSessionSubscribed( QInfinity::BrowserNoteItem &note,
            QInfinity::Session session );
    
    private:
        void setupSignals();
        void insertSession( QInfinity::Session session );

        KTextEditor::Editor *editor;
        QInfinity::BrowserModel *m_browserModel;
        QList<CollabDocument*> m_collabDocuments;
        QMap<KTextEditor::Document*, CollabDocument*> documentCollabDocumentMap;

};

}

#endif

