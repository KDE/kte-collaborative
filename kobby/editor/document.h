#ifndef KOBBY_DOCUMENT_H
#define KOBBY_DOCUMENT_H

#include <KTextEditor/Document>

#include <QObject>

namespace QInfinity
{
    class Session;
}

namespace Kobby
{

/**
 * @brief A base class for interacting with Documents.
 */
class Document
    : public QObject
{

    public:
        enum Type
        {
            KDocument = 1,
            InfText = 2
        };

        Document( KTextEditor::Document &kDocument,
            QObject *parent = 0 );
        virtual ~Document();

        virtual Type type() const;
        KTextEditor::Document *kDocument() const;
        virtual bool save();
        virtual QString name();
    
    private:
        KTextEditor::Document *m_kDocument;

};

class InfTextDocument
    : public Document
{

    public:
        InfTextDocument( KTextEditor::Document &kDocument,
            QInfinity::Session &session,
            QObject *parent = 0 );

        Document::Type type() const;

    private:
        void setupSignals();

        QInfinity::Session *m_session;

};

}

#endif

