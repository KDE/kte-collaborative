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

#ifndef _KOBBY_PLUGIN_H_
#define _KOBBY_PLUGIN_H_

#include "common/connection.h"
#include "manageddocument.h"

#include <KTextEditor/Plugin>
#include <KTextEditor/View>

#include <KJob>
#include <KUrl>
#include <KIO/Job>
#include <KPluginFactory>

#include <libqinfinity/qgsignal.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browseriter.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/textsession.h>

#include <QtCore/QObject>
#include <QStack>

using namespace Kobby;

class KteCollaborativePluginView;

/**
 * @brief The main class of the project, a plugin to KTextEditor.
 *
 * This class is responsible for deciding what documents should be managed,
 * and what views the plugin UI should be added to. All other things
 * should be managed in the appropriate other classes.
 */
class KteCollaborativePlugin : public KTextEditor::Plugin {
Q_OBJECT
public:
    /**
     * @brief Constructor, only to be used internally.
     */
    explicit KteCollaborativePlugin( QObject *parent = 0,
                          const QVariantList &args = QVariantList() );
    virtual ~KteCollaborativePlugin();

    /**
     * @brief Called when a new view is created for a document
     *
     * @param view passed by KTE, the view which was created.
     */
    virtual void addView(KTextEditor::View *view);

    /**
     * @brief Called when a view is removed
     *
     * @param view passed by KTE, the view which was removed.
     */
    virtual void removeView(KTextEditor::View *view);

    /**
     * @brief Called when a new document is added (e.g. user opens a document in kate)
     *
     * @param document The document which was added. URL is probably not set yet.
     */
    virtual void addDocument(KTextEditor::Document* document);

    /**
     * @brief Checks if connections have been established for all managed documents,
     *        and subscribes those which are not yet subscribed (if possible).
     */
    void subscribeNewDocuments();

    /**
     * @brief Get a list of all documents currently being managed by the plugin.
     * Documents are removed from this list if they are disconnected (actively
     * by the user, or for any other reason). All the documents in this list
     * are being synchronized, or are currently being connected.
     * @return const ManagedDocumentList& List of managed documents
     */
    const ManagedDocumentList& managedDocuments() const;

private:
    /**
     * @brief Ensures that a connection for the given URL exists.
     * Gets a Connection from the m_connections hashtable if it exists,
     * or sets it up and returns it otherwise.
     * The connection returned is not necessarily ready to be used,
     * but never null.
     * @param documentUrl The URL to get a connection for. Hostname and port are read from it.
     * @return Kobby::Connection* The connection. Not necessarily established, but never null.
     */
    Connection* ensureConnection(const KUrl& documentUrl);

    /**
     * @brief Returns a unique name for a connection to the host of the given URL
     */
    const QString connectionName(const KUrl& url);

    /**
     * @brief Returns the URLs port, or the default infinity port (6523) if none is set
     */
    unsigned short portForUrl(const KUrl& url);

public slots:
    /**
     * @brief Slot which should be invoked when a browser becomes ready
     *
     * @param browser The browser which became ready
     */
    void browserConnected(const QInfinity::Browser*);

    /**
     * @brief Should be invoked when a connection is prepared (after hostname lookup etc)
     * It will proceed to establish the connection.
     * @param connection The connection which is ready to be established
     */
    void connectionPrepared(Connection*);

    /**
     * @brief Should be invoked when a document will need to be added or removed from the managed list.
     * Depending on the document's URL (inf:// protocol or not), it will add it
     * to or remove it from the m_managedDocuments list of managed documents.
     * @see manageddocuments()
     * @param document The document to check
     */
    void checkManageDocument(KTextEditor::Document*);

    /**
     * @brief Called when text is inserted into a document. For debugging purposes only.
     */
    void textInserted(KTextEditor::Document*, KTextEditor::Range);

    /**
     * @brief Called when text is removed from a document. For debugging purposes only.
     */
    void textRemoved(KTextEditor::Document*, KTextEditor::Range);

    /**
     * @brief Should be called when a document is about to be closed
     * Called when a document is closed or otherwise removed
     * This is a slot because it needs to disconnect from the collaborative
     * server before KTE clears the document's text (which would cause the
     * text to be erased for other users, too)
     * @param document the document which was removed from the editor
     */
    virtual void removeDocument(KTextEditor::Document*);

    /**
     * @brief Should be invoked when a connection was disconnected.
     * It will delete the connection and remove it from the connection map,
     * such that for new documents a new connetion will be established.
     * @param connection The connection which was disconnected.
     */
    void connectionDisconnected(Connection*);

signals:
    //
    /**
     * @brief Emitted when a document becomes managed.
     * The document is probably not yet connected or synchronized.
     * @param document The document which became managed
     */
    void newManagedDocument(ManagedDocument*);

    /**
     * @brief Emitted when a document stops being managed.
     * The document instance is still valid when this is emitted, but will
     * become invalid immediately after that.
     * @param document The document which became unmanaged
     */
    void removedManagedDocument(ManagedDocument*);

private:
    // List of managed douments
    ManagedDocumentList m_managedDocuments;
    QInfinity::BrowserModel* m_browserModel;
    QInfinity::NotePlugin* m_textPlugin;
    QInfinity::CommunicationManager* m_communicationManager;
    // Maps connection names to connection instances;
    // the connection name is host:port, get it with connectionName(url)
    QHash<QString, Kobby::Connection*> m_connections;
    // Maps KTextEditor::View instances to KobbyPluginView instances.
    QMap<KTextEditor::View*, KteCollaborativePluginView*> m_views;
};

K_PLUGIN_FACTORY_DECLARATION(KteCollaborativePluginFactory)

#endif // _KOBBY_PLUGIN_H_

// kate: space-indent on; indent-width 4; replace-tabs on;
