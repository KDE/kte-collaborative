/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KOBBY_DOCUMENTMANAGER_H
#define KOBBY_DOCUMENTMANAGER_H
#include "kobbycommon_export.h"

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
class KOBBYCOMMON_EXPORT DocumentBuilder
    : public QObject
{
    Q_OBJECT

    public:
        DocumentBuilder( QInfinity::BrowserModel& browserModel, QObject* parent = 0 );
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
        void documentCreated( Document &document,
            const QInfinity::BrowserIter *iter = 0 );

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
        void slotBrowserAdded( QInfinity::Browser &browser );
    
    private:
        void sessionSynchronized( QInfinity::Session *session );

        QInfinity::BrowserModel *m_browserModel;
        QHash<QInfinity::Session*, QInfinity::SessionProxy*> sessionToProxy;

};

}

#endif

