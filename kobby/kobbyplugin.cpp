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


K_PLUGIN_FACTORY( KobbyPluginFactory, registerPlugin<KobbyPlugin>(); )
K_EXPORT_PLUGIN( KobbyPluginFactory( KAboutData( "ktexteditor_kobby", "ktexteditor_plugins",
                                          ki18n( "Collaborative Editing" ), "1.0", ki18n("Collaborative Editing"), KAboutData::License_LGPL_V2 ) ) )

KobbyPlugin::KobbyPlugin( QObject *parent, const QVariantList& )
  : KTextEditor::Plugin ( parent )
  , m_isConnected(false)
  , m_browserReady(false)
  , m_docBuilder(0)
  , m_session(0)
{
    kDebug() << "loading kobby plugin";
    QInfinity::init();
    m_browserModel = new QInfinity::BrowserModel( this );
    m_browserModel->setItemFactory( new Kobby::ItemFactory( this ) );
    kDebug() << "ok";
}

KobbyPlugin::~KobbyPlugin()
{
}

void KobbyPlugin::connectionPrepared()
{
    kDebug() << "connection prepared, establishing connection";
    m_browserModel->addConnection(*(m_connection->xmppConnection()), "Test connection");
    foreach ( QInfinity::Browser* browser, m_browserModel->browsers() ) {
        QObject::connect(browser, SIGNAL(connectionEstablished(const QInfinity::Browser*)),
                         this, SLOT(browserConnected(const QInfinity::Browser*)), Qt::UniqueConnection);
    }
    m_connection->open();
}

void KobbyPlugin::userJoinCompleted(QPointer< QInfinity::User > user)
{
    kDebug() << "user join completed";
    foreach ( ManagedDocument* doc, m_managedDocuments ) {
        doc->m_textBuffer->setUser(user);
    }
}

void KobbyPlugin::connected(Kobby::Connection* connection)
{
    kDebug() << "connection established!";
    m_isConnected = true;
    subscribeNewDocuments();
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

void KobbyPlugin::addView(KTextEditor::View *view)
{
    kDebug() << "adding view; document url:" << view->document()->url();
    // set up document as soon as its url gets set correctly
    connect(view->document(), SIGNAL(documentUrlChanged(KTextEditor::Document*)),
            this, SLOT(documentUrlChanged(KTextEditor::Document*)));
}

void KobbyPlugin::removeView(KTextEditor::View *view)
{
  foreach (KobbyPluginView *pluginView, m_views) {
    if (pluginView->view() == view) {
      m_views.removeAll(pluginView);
      delete pluginView;
      break;
    }
  }
}

void KobbyPlugin::addDocument(KTextEditor::Document* document)
{
    kDebug() << "adding document" << document << document->url();
    m_managedDocuments.append(new ManagedDocument(document, m_browserModel));
}

void KobbyPlugin::removeDocument(KTextEditor::Document* document)
{
    foreach ( ManagedDocument* doc, m_managedDocuments ) {
        if ( doc->document() == document ) {
            m_managedDocuments.removeOne(doc);
            delete doc;
        }
    }
}

void KobbyPlugin::documentUrlChanged(KTextEditor::Document* document)
{
    kDebug() << "new url:" << document->url() << document;
    if ( document->url().protocol() != "inf" ) {
        kDebug() << "not a collaborative document:" << document->url().url();
        return;
    }
    kDebug() << "initializing collaborative session for document" << document->url();
    m_connection = new Kobby::Connection("localhost", 6523, this);
    connect(m_connection, SIGNAL(connected(Connection*)),
            this, SLOT(connected(Connection*)));
    connect(m_connection, SIGNAL(ready()),
            this, SLOT(connectionPrepared()));
    m_connection->prepare();

    connect(document, SIGNAL(textInserted(KTextEditor::Document*, KTextEditor::Range)),
            this, SLOT(textInserted(KTextEditor::Document*, KTextEditor::Range)));
    connect(document, SIGNAL(textRemoved(KTextEditor::Document*,KTextEditor::Range)),
            this, SLOT(textRemoved(KTextEditor::Document*,KTextEditor::Range)));

    if ( ! m_docBuilder ) {
        // TODO this setup method sucks. Totally the wrong place to do it.
        // But we don't get the editor instance earlier, so need to investigate why it is needed.
        m_docBuilder = new Kobby::DocumentBuilder( *(document->editor()), *m_browserModel, this );

        KDocumentTextBuffer* buffer = new Kobby::KDocumentTextBuffer(document, "utf-8");
        foreach ( ManagedDocument* doc, m_managedDocuments ) {
            if ( doc->document() == document ) {
                doc->m_textBuffer = buffer;
            }
        }
        m_textPlugin = new Kobby::NotePlugin( document->editor(), buffer,
                                              this );
        m_communicationManager = new QInfinity::CommunicationManager();
        m_browserModel->addPlugin( *m_textPlugin );
    }

    subscribeNewDocuments();

//     browser->subscribeSession(browser);
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

ManagedDocument::ManagedDocument(KTextEditor::Document* document, QInfinity::BrowserModel* model)
    : QObject()
    , m_document(document)
    , m_browserModel(model)
    , m_subscribed(false)
{
    kDebug() << "now managing document" << document << document->url();
}

ManagedDocument::~ManagedDocument()
{
    
}

void ManagedDocument::unsubscribe()
{
    kDebug() << "should unsubscribe document";
}

bool ManagedDocument::isSubscribed()
{
    return m_subscribed;
}

void ManagedDocument::subscribe()
{
    if ( m_document->url().protocol() != "inf" ) {
        // TODO URGENT urghghh
        return;
    }
    kDebug() << "beginning subscription for" << m_document->url();
    // TODO URGENT browsers.first is wrong
    IterLookupHelper* helper = new IterLookupHelper(m_document->url().path(KUrl::RemoveTrailingSlash),
                                                    m_browserModel->browsers().first());
    connect(helper, SIGNAL(done(QInfinity::BrowserIter)),
            this, SLOT(finishSubscription(QInfinity::BrowserIter)));
    helper->begin();
}

void ManagedDocument::subscriptionDone(QInfinity::BrowserIter iter, QPointer< QInfinity::SessionProxy > proxy)
{
    kDebug() << "subscription done, waiting for sync" << proxy->session()->status() << QInfinity::Session::Running;
    m_proxy = proxy;
    QObject::connect(proxy->session(), SIGNAL(statusChanged()),
                     this, SLOT(sessionStatusChanged()));
}

void ManagedDocument::sessionStatusChanged()
{
    kDebug() << "session status changed to " << m_proxy->session()->status();
    if ( m_proxy->session()->status() != QInfinity::Session::Running ) {
        kDebug() << "not running, ignoring event";
        return;
    }
    QTimer::singleShot(0, this, SLOT(joinUser()));
}

void ManagedDocument::joinUser()
{
//     QInfinity::UserRequest* request = QInfinity::TextSession::joinUser(m_proxy,
//                 *dynamic_cast<QInfinity::TextSession*>(m_proxy->session().data()),
//                                                        "b00n" + QString::number(QTime::currentTime().second()), 0.4);
//     QObject::connect(request, SIGNAL(finished(QPointer<QInfinity::User>)),
//                      this, SLOT(userJoinCompleted(QPointer<QInfinity::User>)));
}

void ManagedDocument::userJoinCompleted(QPointer< QInfinity::User > )
{
    // delete the join request
//     QObject::sender()->deleteLater();
//     kDebug() << "whee, new user joined";
}

void ManagedDocument::finishSubscription(QInfinity::BrowserIter iter)
{
    // delete the lookup helper
    QObject::sender()->deleteLater();
    kDebug() << "finishing subscription with iter " << iter.path();
    QPointer< QInfinity::Browser > browser = iter.browser();
    QObject::connect(browser.data(), SIGNAL(subscribeSession(QInfinity::BrowserIter,QPointer<QInfinity::SessionProxy>)),
                     this, SLOT(subscriptionDone(QInfinity::BrowserIter,QPointer<QInfinity::SessionProxy>)));
    browser->subscribeSession(iter);
}

// kate: space-indent on; indent-width 4; replace-tabs on;
