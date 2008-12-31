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
        CollabDocument( QInfinity::Document &infDocument,
            KTextEditor::Document &document,
            QObject *parent = 0 );
        ~CollabDocument();    

    private Q_SLOTS:
        void slotLocalTextInserted( KTextEditor::Document *document,
            const KTextEditor::Range &range );
        void slotInsertText( unsigned int pos,
            Infinity::TextChunk textChunk,
            Infinity::User *user );

    private:
        void setupActions();

        QInfinity::Document *m_infDocument;
        KTextEditor::Document *m_kDocument;

};

}

#endif

