/* This file is part of the Kobby plugin
 * Copyright (C) 2013 Sven Brauch <svenbrauch@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef KOBBY_MANAGEDDOCUMENT_H
#define KOBBY_MANAGEDDOCUMENT_H

#include <QObject>

#include <KTextEditor/Document>

#include <libqinfinity/browsermodel.h>
#include <libqinfinity/sessionproxy.h>
#include <libqinfinity/browseriter.h>
#include <libqinfinity/session.h>

#include <common/document.h>
#include <common/connection.h>

class DocumentChangeTracker;
using Kobby::Connection;
using Kobby::Document;

/**
 * @brief Describes an instance of a document managed by the plugin.
 * This class ties together a KTextEditor::Document instance to all the
 * collaborative editing backend classes.
 */
class ManagedDocument : public QObject {
Q_OBJECT
public:
    /**
     * @brief Create a new managed document instance.
     * Doing so will initiate the synchronization process for the given KTE::Document.
     * @param document The document from KTE to be synchronized, usually retrieved from the addDocument() method of the plugin
     * @param browserModel The browser model; the plugin instance knows about it
     * @param plugin The note plugin; the plugin instance knows about it
     * @param connection The connection; the plugin instance knows about it
     * @param parent parent object for lifetime management purposes
     */
    ManagedDocument(KTextEditor::Document* document, QInfinity::BrowserModel* browserModel,
                    QInfinity::NotePlugin* plugin, Kobby::Connection* connection, QObject* parent = 0);

    virtual ~ManagedDocument();

    /**
     * @brief Subscribe to this document, to get notified of changes and also upload own changes.
     * This operation is asynchroneous.
     */
    void subscribe();

    /**
     * @brief Unsubscribe this document, to stop receiving and sending updates.
     */
    void unsubscribe();

    /**
     * @brief Tells whether this document is currently being synchronized or not.
     * @return true of synchronizing, false otherwise.
     */
    bool isSubscribed() const;

    /**
     * @brief Get the session status of this document's session.
     * This is "Running" if the document is currently receiving and uploading changes.
     * "Synchronizing" just occurs on start-up (initial synchronization).
     */
    QInfinity::Session::Status sessionStatus() const;

    inline KTextEditor::Document* document() const {
        return m_document;
    };
    inline Kobby::InfTextDocument* infTextDocument() const {
        return m_infDocument;
    };
    inline Kobby::KDocumentTextBuffer* textBuffer() const {
        return m_textBuffer;
    };
    inline void setLocalSavePath(const QString& url) {
        m_localSavePath = url;
    };
    inline const QString& localSavePath() const {
        return m_localSavePath;
    };
    inline DocumentChangeTracker* changeTracker() const {
        return m_changeTracker;
    };

    /**
     * @brief Returns the Browser for the document's connection
     */
    QInfinity::Browser* browser() const;

    /**
     * @brief Returns the Connection for this document
     */
    Kobby::Connection* connection() const;

    /**
     * @brief Returns the user table for this document
     */
    QInfinity::UserTable* userTable() const;

    /**
     * @brief Save a copy of this document to the URL set by setLocalSaveUrl().
     * If this method succeeds, it'll set the part status to "not modified".
     * @return true if saved successfully, else false.
     */
    bool saveCopy() const;

public slots:
    /**
     * @brief Invoked when a subscription is ready to be finished (i.e. the iter is known)
     * @param iter The browser iter (like an "internal URI") for the document
     */
    void finishSubscription(QInfinity::BrowserIter iter);

    /**
     * @brief Invoked when a subscription was done; will initiate synchronizing the document.
     */
    void subscriptionDone(QInfinity::BrowserIter, QPointer<QInfinity::SessionProxy>);

    /**
     * @brief Invoked when the session status changes (e.g. Synchronizing -> Running, or Running -> Disconnected).
     */
    void sessionStatusChanged();

    /**
     * @brief Invoked when the document is completely synchronized; it's now fully ready to be used.
     */
    void synchronizationComplete(Document*);

    /**
     * @brief Invoked when a connection breaks.
     */
    void disconnected(Connection*);

signals:
    /**
     * @brief Emitted when a document is completely synchronized and ready to be used (user can start typing etc).
     * @param document The document which became ready.
     */
    void documentReady(ManagedDocument* document);

    /**
     * @brief Emitted when synchronization of a document begins.
     * @param document The document for which synchronization has begun
     */
    void synchronizationBegins(ManagedDocument* document);

private:
    Kobby::KDocumentTextBuffer* m_textBuffer;
    KTextEditor::Document* m_document;
    QInfinity::BrowserModel* m_browserModel;
    QInfinity::NotePlugin* m_notePlugin;
    Kobby::Connection* m_connection;
    bool m_subscribed;
    QPointer< QInfinity::SessionProxy > m_proxy;
    Kobby::InfTextDocument* m_infDocument;
    // id of the browser iter for this document, for checking whether signals are meant for it
    unsigned int m_iterId;
    // status of this session (synchronizing, running etc)
    QInfinity::Session::Status m_sessionStatus;
    // local URL to copy the document to if requested
    QString m_localSavePath;
    DocumentChangeTracker* m_changeTracker;
};

typedef QMap<KTextEditor::Document*, ManagedDocument*> ManagedDocumentList;

#endif
