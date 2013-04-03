/* This file is part of the KDE libraries
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

#include "kobbyplugin.h"

#include <ktexteditor/document.h>
#include <ktexteditor/attribute.h>
#include <ktexteditor/searchinterface.h>
#include <ktexteditor/movinginterface.h>
#include <ktexteditor/movingrange.h>
#include <ktexteditor/configinterface.h>

#include <assert.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kfiledialog.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <KTextEditor/Editor>

#include "common/connection.h"
#include "common/document.h"
#include "editor/collabsession.h"
#include "common/itemfactory.h"
#include "common/documentbuilder.h"
#include "editor/remotebrowserview.h"
#include "common/noteplugin.h"
#include "common/documentmodel.h"
#include <createitemdialog.h>
#include <createconnectiondialog.h>

#include <libqinfinity/communicationjoinedgroup.h>
#include <libqinfinity/init.h>
#include <libqinfinity/user.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browseritemfactory.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/noteplugin.h>
#include <libqinfinity/xmlconnection.h>
#include <libqinfinity/xmppconnection.h>
#include <libqinfinity/userrequest.h>

#include <kparts/part.h>
#include <QApplication>
#include <QTimer>

// This is the default port for infinoted.
static int defaultPort = 6523;

K_PLUGIN_FACTORY( KobbyPluginFactory, registerPlugin<KobbyPlugin>(); )
K_EXPORT_PLUGIN( KobbyPluginFactory( KAboutData( "ktexteditor_kobby", "ktexteditor_plugins",
                                          ki18n( "Collaborative Editing" ), "1.0", ki18n("Collaborative Editing"), KAboutData::License_LGPL_V2 ) ) )

KobbyPlugin::KobbyPlugin( QObject *parent, const QVariantList& )
  : KTextEditor::Plugin ( parent )
  , m_isConnected(false)
  , m_browserReady(false)
  , m_session(0)
{
    kDebug() << "loading kobby plugin";
    QInfinity::init();
    m_browserModel = new QInfinity::BrowserModel( this );
    m_browserModel->setItemFactory( new Kobby::ItemFactory( this ) );
    m_textPlugin = new Kobby::NotePlugin( this );
    m_communicationManager = new QInfinity::CommunicationManager();
    m_browserModel->addPlugin( *m_textPlugin );
    kDebug() << "ok";
}

KobbyPlugin::~KobbyPlugin()
{
}

void KobbyPlugin::connectionPrepared(Connection* connection)
{
    kDebug() << "connection prepared, establishing connection";
    m_browserModel->addConnection(connection->xmppConnection(), connection->name());
    foreach ( QInfinity::Browser* browser, m_browserModel->browsers() ) {
        QObject::connect(browser, SIGNAL(connectionEstablished(const QInfinity::Browser*)),
                         this, SLOT(browserConnected(const QInfinity::Browser*)), Qt::UniqueConnection);
    }
    connection->open();
}

void KobbyPlugin::connected(Kobby::Connection* connection)
{
    kDebug() << "connection established!";
    m_isConnected = true;
}

void KobbyPlugin::browserConnected(const QInfinity::Browser* )
{
    kDebug() << "browser connected";
    // TODO differentiate which connection the browser even belongs to!
    m_browserReady = true;
    subscribeNewDocuments();
}

void KobbyPlugin::subscribeNewDocuments()
{
    kDebug() << "subscribing new documents; connected (tcp/browser):" << m_isConnected << m_browserReady;
    if ( ! m_browserReady ) {
        return;
    }
    foreach ( ManagedDocument* document, m_managedDocuments ) {
        if ( ! document->isSubscribed() ) {
            document->subscribe();
        }
    }
}

void KobbyPlugin::addDocument(KTextEditor::Document* document)
{
    kDebug() << "add document" << document << document->url();
    // TODO this is not good semantically
    documentUrlChanged(document);
}

void KobbyPlugin::removeDocument(KTextEditor::Document* document)
{
    ManagedDocument* doc = m_managedDocuments.findDocument(document);
    if ( doc ) {
        m_managedDocuments.removeAll(doc);
        delete doc;
    }
    else {
        kWarning() << "tried to remove document" << document << "which is not being managed";
    }
}

void KobbyPlugin::documentUrlChanged(KTextEditor::Document* document)
{
    kDebug() << "new url:" << document->url() << document;
    if ( document->url().protocol() != "inf" ) {
        kDebug() << "not a collaborative document:" << document->url().url();
        if ( m_managedDocuments.isManaged(document) ) {
            kDebug() << "removing document" << document << "from manager";
            ManagedDocument* doc = m_managedDocuments.findDocument(document);
            m_managedDocuments.removeAll(doc);
            delete doc;
        }
        return;
    }
    if ( m_managedDocuments.isManaged(document) ) {
        kDebug() << document->url() << "is already being managed.";
        return;
    }
    kDebug() << "initializing collaborative session for document" << document->url();

    Connection* connection = eventuallyAddConnection(document->url());

    ManagedDocument* managed = new ManagedDocument(document, m_browserModel, m_textPlugin, connection, this);
    m_managedDocuments.append(managed);

    connect(document, SIGNAL(textInserted(KTextEditor::Document*, KTextEditor::Range)),
            this, SLOT(textInserted(KTextEditor::Document*, KTextEditor::Range)), Qt::UniqueConnection);
    connect(document, SIGNAL(textRemoved(KTextEditor::Document*,KTextEditor::Range)),
            this, SLOT(textRemoved(KTextEditor::Document*,KTextEditor::Range)), Qt::UniqueConnection);

    KDocumentTextBuffer* buffer = new Kobby::KDocumentTextBuffer(document, "utf-8");

    subscribeNewDocuments();
}

Connection* KobbyPlugin::eventuallyAddConnection(const KUrl& documentUrl)
{
    int port = documentUrl.port();
    port = port == -1 ? defaultPort : port;
    QString connectionName = documentUrl.host() + ":" + port;
    if ( ! m_connections.contains(connectionName) ) {
        kDebug() << "adding connection" << connectionName << "because it doesn't exist";
        Connection* c = new Kobby::Connection(documentUrl.host(), port, this);
        connect(c, SIGNAL(connected(Connection*)),
                this, SLOT(connected(Connection*)));
        connect(c, SIGNAL(ready(Connection*)),
                this, SLOT(connectionPrepared(Connection*)));
        m_connections[connectionName] = c;
        c->prepare();
        return c;
    }
    else {
        kDebug() << "connection" << connectionName << "requested but it exists already";
    }
    return m_connections[connectionName];
}

void KobbyPlugin::addView(KTextEditor::View* view)
{
    connect(view->document(), SIGNAL(documentUrlChanged(KTextEditor::Document*)),
            this, SLOT(documentUrlChanged(KTextEditor::Document*)));
}

void KobbyPlugin::removeView(KTextEditor::View* view)
{
    kDebug() << "removing view" << view;
}

KobbyPluginView::KobbyPluginView( KTextEditor::View *view, Kobby::Connection* /*connection*/)
  : QObject( view )
{
    setObjectName("kobby-plugin");
    m_view = view;

    connect(view, SIGNAL(selectionChanged(KTextEditor::View*)), this, SLOT(selectionChanged()));
}

KobbyPluginView::~KobbyPluginView()
{

}

void KobbyPlugin::textInserted(KTextEditor::Document* doc, KTextEditor::Range range)
{
    kDebug() << "text inserted:" << range << doc->textLines(range) << doc;
}

void KobbyPlugin::textRemoved(KTextEditor::Document* doc, KTextEditor::Range range)
{
    kDebug() << "text removed:" << range << doc->textLines(range) << doc;
}

KTextEditor::View* KobbyPluginView::view() const
{
    return m_view;
}

void KobbyPluginView::selectionChanged()
{
    kDebug() << "plugin: selection changed" << "in view" << this;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
