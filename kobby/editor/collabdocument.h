#ifndef KOBBY_COLLABDOCUMENT_H
#define KOBBY_COLLABDOCUMENT_H

#include <QObject>

namespace QInfinity
{
    class Document;
}

namespace Infinity
{
    class User;
    class TextChunk;
    class Session;
}

namespace KTextEditor
{
    class Document;
    class Range;
}

namespace Kobby
{

class CollabDocument
    : public QObject
{

    public:
        CollabDocument( Infinity::Session &session,
            KTextEditor::Document &document,
            QObject *parent = 0 );
        ~CollabDocument();

        KTextEditor::Document *kDocument() const;

    private Q_SLOTS:
        void slotLocalTextInserted( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void slotInsertText( unsigned int pos,
            Infinity::TextChunk textChunk,
            Infinity::User *user );

    private:
        void setupActions();

        QInfinity::Document *m_infDocument;
        Infinity::Session *m_infSession;
        KTextEditor::Document *m_kDocument;

};

}

#endif

