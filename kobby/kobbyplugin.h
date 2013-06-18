/* This file is part of the Kobby
   Copyright (C) 2013 Sven Brauch <svenbrauch@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#ifndef _KOBBY_PLUGIN_H_
#define _KOBBY_PLUGIN_H_

#include <ktexteditor/plugin.h>
#include <ktexteditor/view.h>

#include <QtCore/QObject>
#include <QStack>
#include <kjob.h>
#include <kurl.h>
#include <kio/job.h>
#include <kpluginfactory.h>

#include <libqinfinity/qgsignal.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browseriter.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/textsession.h>

#include "common/connection.h"
#include "common/documentbuilder.h"
#include "manageddocument.h"

using namespace Kobby;

class KobbyPluginView;

class KobbyPlugin : public KTextEditor::Plugin {
Q_OBJECT
public:
    explicit KobbyPlugin( QObject *parent = 0,
                        const QVariantList &args = QVariantList() );
    virtual ~KobbyPlugin();

    // Called when a new view is created for a document
    virtual void addView(KTextEditor::View *view);
    // Called when a view is removed
    virtual void removeView(KTextEditor::View *view);
    // Called when a new document is added (e.g. user opens a document in kate)
    virtual void addDocument(KTextEditor::Document* document);
    // Called when a document is closed or otherwise removed
    virtual void removeDocument(KTextEditor::Document* document);

    // Checks if connections have been established for all managed documents,
    // and subscribes those which are not yet subscribed (if possible).
    void subscribeNewDocuments();
    // access to managed documents for unit tests
    const ManagedDocumentList& managedDocuments() const;

private:
    // Gets a Connection from the m_connections hashtable if it exists,
    // or sets it up and returns it otherwise.
    // The connection returned is not necessarily ready to be used,
    // but never null.
    Connection* eventuallyAddConnection(const KUrl& documentUrl);
    // Returns a unique name for a connection to the host of the given URL
    const QString connectionName(const KUrl& url);
    // Returns the URLs port, or the default infinity port if none is set
    unsigned short portForUrl(const KUrl& url);

    ManagedDocumentList m_managedDocuments;
    QInfinity::BrowserModel* m_browserModel;
    QInfinity::NotePlugin* m_textPlugin;
    QInfinity::CommunicationManager* m_communicationManager;
    // Maps connection names to connection instances;
    // the connection name is host:port, get it with connectionName(url)
    // TODO use QHash?
    QHash<QString, Kobby::Connection*> m_connections;

public slots:
    // This is called when the browser is ready.
    void browserConnected(const QInfinity::Browser*);
    // Called when a connection is prepared (after hostname lookup etc)
    void connectionPrepared(Connection*);
    // Should be called whenever a significant property of a document
    // changes, or a new document is added. It will eventually
    // add that document to the plugin.
    void eventuallyManageDocument(KTextEditor::Document*);
    void textInserted(KTextEditor::Document*, KTextEditor::Range);
    void textRemoved(KTextEditor::Document*, KTextEditor::Range);
};

#endif // _KOBBY_PLUGIN_H_

// kate: space-indent on; indent-width 4; replace-tabs on;
