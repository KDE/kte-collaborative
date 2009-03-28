#ifndef KOBBY_DOCUMENT_H
#define KOBBY_DOCUMENT_H

#include <KTextEditor/Document>

#include <QObject>
#include <QPointer>

#include <glib/gerror.h>

namespace QInfinity
{
    class Session;
    class SessionProxy;
    class User;
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
    Q_OBJECT;

    public:
        InfTextDocument( KTextEditor::Document &kDocument,
            QPointer<QInfinity::SessionProxy> sesisonProxy,
            QObject *parent = 0 );

        Document::Type type() const;

    private Q_SLOTS:
        void sessionRunning();
        void userJoined( QPointer<QInfinity::User> user );
        void userJoinFailed( GError *error );

    private:
        QPointer<QInfinity::SessionProxy> m_sessionProxy;
        QPointer<QInfinity::User> m_user;

};

}

#endif

