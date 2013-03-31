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


K_PLUGIN_FACTORY( KobbyPluginFactory, registerPlugin<KobbyPlugin>(); )
K_EXPORT_PLUGIN( KobbyPluginFactory( KAboutData( "ktexteditor_kobby", "ktexteditor_plugins",
                                                              ki18n("Collaborative Editing"), "1.0", ki18n("Collaborative Editing"), KAboutData::License_LGPL_V2 ) ) )

KobbyPlugin::KobbyPlugin( QObject *parent, const QVariantList& )
  : KTextEditor::Plugin ( parent )
  , m_isConnected(false)
{
    kDebug() << "loading kobby plugin";
    QInfinity::init();

    m_connection = new Kobby::Connection("localhost", 6523, this);
    connect(m_connection, SIGNAL(connected(Connection*)),
            this, SLOT(connected(Connection*)));
    m_connection->open();
    kDebug() << "ok";
}

KobbyPlugin::~KobbyPlugin()
{
}

void KobbyPlugin::connected(Kobby::Connection*)
{
    qDebug() << "connection established!";
    m_isConnected = true;
    foreach ( KobbyPluginView* view, m_views ) {
        view->connected(m_connection);
    }
}

void KobbyPlugin::addView(KTextEditor::View *view)
{
    kDebug() << "adding view" << view;
    KobbyPluginView *nview = new KobbyPluginView(view, m_connection);
    if ( m_isConnected ) {
        nview->connected(m_connection);
    }
    m_views.append (nview);
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
    kDebug() << "opening document";
    connect(document, SIGNAL(documentUrlChanged(KTextEditor::Document*)),
            this, SLOT(documentUrlChanged(KTextEditor::Document*)));
}

void KobbyPlugin::documentUrlChanged(KTextEditor::Document* document)
{
    kDebug() << "new url:" << document->url();
}

KobbyPluginView::KobbyPluginView( KTextEditor::View *view, Kobby::Connection* connection)
  : QObject( view )
  , m_connection(connection)
{
    setObjectName("kobby-plugin");
    m_view = view;

    connect(view, SIGNAL(selectionChanged(KTextEditor::View*)), this, SLOT(selectionChanged()));
    connect(view->document(), SIGNAL(textInserted(KTextEditor::Document*, KTextEditor::Range)),
            this, SLOT(textInserted(KTextEditor::Document*, KTextEditor::Range)));
    connect(view->document(), SIGNAL(textRemoved(KTextEditor::Document*,KTextEditor::Range)),
            this, SLOT(textRemoved(KTextEditor::Document*,KTextEditor::Range)));


    m_browserModel = new QInfinity::BrowserModel( this );
    m_browserModel->setItemFactory( new Kobby::ItemFactory( this ) );
    m_docBuilder = new Kobby::DocumentBuilder( *(view->document()->editor()), *m_browserModel, this );

    m_textPlugin = new Kobby::NotePlugin( *m_docBuilder, this );
    m_browserModel->addPlugin( *m_textPlugin );
}

void KobbyPluginView::connected(Kobby::Connection* connection)
{
    Kobby::RemoteBrowserProxy* remoteBrowserView = new Kobby::RemoteBrowserProxy( *m_textPlugin, *m_browserModel, 0 );
    m_browserModel->addConnection(*static_cast<QInfinity::XmlConnection*>(m_connection->xmppConnection()), "Test connection");
    connect(&remoteBrowserView->remoteView(), SIGNAL(openItem(QModelIndex)),
            m_docBuilder, SLOT(openInfDocmuent(QModelIndex)));
    remoteBrowserView->show();
}

KobbyPluginView::~KobbyPluginView()
{

}

void KobbyPluginView::textInserted(KTextEditor::Document* doc, KTextEditor::Range range)
{
    kDebug() << "text inserted:" << range << doc->textLines(range);
}

void KobbyPluginView::textRemoved(KTextEditor::Document* doc, KTextEditor::Range range)
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

// kate: space-indent on; indent-width 4; replace-tabs on;
