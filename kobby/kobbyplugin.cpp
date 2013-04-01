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

#include "editor/connection.h"
#include "editor/document.h"
#include "editor/collabsession.h"
#include "editor/itemfactory.h"
#include "editor/documentbuilder.h"
#include "editor/remotebrowserview.h"
#include "editor/noteplugin.h"
#include "editor/documentmodel.h"
#include <createitemdialog.h>
#include <createconnectiondialog.h>

#include <libqinfinity/init.h>
#include <libqinfinity/user.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browseritemfactory.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/noteplugin.h>
#include <libqinfinity/xmlconnection.h>
#include <libqinfinity/xmppconnection.h>

#include <kparts/part.h>


K_PLUGIN_FACTORY( KobbyPluginFactory, registerPlugin<KobbyPlugin>(); )
K_EXPORT_PLUGIN( KobbyPluginFactory( KAboutData( "ktexteditor_kobby", "ktexteditor_plugins",
                                                              ki18n("Collaborative Editing"), "1.0", ki18n("Collaborative Editing"), KAboutData::License_LGPL_V2 ) ) )

KobbyPlugin::KobbyPlugin( QObject *parent, const QVariantList& )
  : KTextEditor::Plugin ( parent )
  , m_isConnected(false)
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

void KobbyPlugin::connected(Kobby::Connection* connection)
{
    m_browserModel->addConnection(*(connection->xmppConnection()), "Test connection");
    qDebug() << "connection established!";
    m_isConnected = true;
    subscribeNewDocuments();
}

void KobbyPlugin::subscribeNewDocuments()
{
    kDebug() << "subscribing new documents; connected:" << m_isConnected;
    if ( ! m_isConnected ) {
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
    kDebug() << "adding document" << document;
    m_managedDocuments.append(new ManagedDocument(document, m_browserModel));
}

void KobbyPlugin::documentUrlChanged(KTextEditor::Document* document)
{
    kDebug() << "new url:" << document->url();
    if ( document->url().protocol() != "inf" ) {
        kDebug() << "not a collaborative document:" << document->url().url();
        return;
    }
    kDebug() << "initializing collaborative session for document" << document->url();
    m_connection = new Kobby::Connection("localhost", 6523, this);
    connect(m_connection, SIGNAL(connected(Connection*)),
            this, SLOT(connected(Connection*)));
    m_connection->open();

    connect(document, SIGNAL(textInserted(KTextEditor::Document*, KTextEditor::Range)),
            this, SLOT(textInserted(KTextEditor::Document*, KTextEditor::Range)));
    connect(document, SIGNAL(textRemoved(KTextEditor::Document*,KTextEditor::Range)),
            this, SLOT(textRemoved(KTextEditor::Document*,KTextEditor::Range)));

    m_docBuilder = new Kobby::DocumentBuilder( *(document->editor()), *m_browserModel, this );

    m_textPlugin = new Kobby::NotePlugin( document->editor(), this );
    m_browserModel->addPlugin( *m_textPlugin );
    kDebug() << "item count:" << m_browserModel->rowCount();

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
    kDebug() << "text inserted:" << range << doc->textLines(range);
}

void KobbyPlugin::textRemoved(KTextEditor::Document* doc, KTextEditor::Range range)
{
    kDebug() << "text removed:" << range << doc->textLines(range);
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
    kDebug() << "beginning subscription";
    // TODO browsers.first is wrong
    IterLookupHelper* helper = new IterLookupHelper(m_document->url().path(KUrl::RemoveTrailingSlash),
                                                    m_browserModel->browsers().first());
    connect(helper, SIGNAL(done(QInfinity::BrowserIter)),
            this, SLOT(finishSubscription(QInfinity::BrowserIter)));
    helper->begin();
}

void ManagedDocument::finishSubscription(QInfinity::BrowserIter iter)
{
    // delete the lookup helper
    QObject::sender()->deleteLater();
    kDebug() << "finishing subscription";
}

// kate: space-indent on; indent-width 4; replace-tabs on;
