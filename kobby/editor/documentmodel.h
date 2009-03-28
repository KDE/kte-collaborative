#ifndef KOBBY_DOCUMENT_MODEL_H
#define KOBBY_DOCUMENT_MODEL_H

#include <QStandardItemModel>
#include <QStandardItem>

namespace Kobby
{

class Document;

class DocumentItem
    : public QObject
    , public QStandardItem
{
    public:
        enum Type
        {
            KobbyDocument = 1001
        };

        DocumentItem( Document &doc );
        ~DocumentItem();

        int type() const;
        Document &document() const;

    private:
        Document *m_document;

};

class DocumentModel
    : public QStandardItemModel
{
    Q_OBJECT;

    public:
        DocumentModel( QObject *parent = 0 );

    Q_SIGNALS:
        void documentAdded( Document &document );
        void documentAboutToBeRemoved( Document &document );

    public Q_SLOTS:
        void insertDocument( Document &document );

    private Q_SLOTS:
        void slotRowsAboutRemoved( const QModelIndex &parent,
            int start, int end );

};

}

#endif

