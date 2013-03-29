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


K_PLUGIN_FACTORY( KobbyPluginFactory, registerPlugin<KobbyPlugin>(); )
K_EXPORT_PLUGIN( KobbyPluginFactory( KAboutData( "ktexteditor_kobby", "ktexteditor_plugins",
                                                              ki18n("Collaborative Editing"), "1.0", ki18n("Collaborative Editing"), KAboutData::License_LGPL_V2 ) ) )


KobbyPlugin::KobbyPlugin( QObject *parent, const QVariantList& )
  : KTextEditor::Plugin ( parent )
{
    qDebug() << "loading kobby plugin";
}

KobbyPlugin::~KobbyPlugin()
{
}

void KobbyPlugin::addView(KTextEditor::View *view)
{
  KobbyPluginView *nview = new KobbyPluginView (view);
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

KobbyPluginView::KobbyPluginView( KTextEditor::View *view)
  : QObject( view )
{
  setObjectName("kobby-plugin");

  m_view = view;

  connect(view, SIGNAL(selectionChanged(KTextEditor::View*)), this, SLOT(selectionChanged()));
}

KobbyPluginView::~KobbyPluginView()
{

}

KTextEditor::View* KobbyPluginView::view() const
{
  return m_view;
}

void KobbyPluginView::selectionChanged()
{
  qDebug() << "plugin: selection changed";
}

// kate: space-indent on; indent-width 4; replace-tabs on;
