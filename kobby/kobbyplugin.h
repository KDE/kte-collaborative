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

#ifndef _KOBBY_PLUGIN_H_
#define _KOBBY_PLUGIN_H_

#include <ktexteditor/plugin.h>
#include <ktexteditor/view.h>

#include <QtCore/QObject>
#include <kjob.h>
#include <kurl.h>
#include <kio/job.h>
#include "editor/connection.h"
#include "editor/documentbuilder.h"
#include <libqinfinity/browsermodel.h>

using namespace Kobby;

class KobbyPlugin : public KTextEditor::Plugin
{
Q_OBJECT

public:
    explicit KobbyPlugin( QObject *parent = 0,
                        const QVariantList &args = QVariantList() );
    virtual ~KobbyPlugin();

    virtual void addView(KTextEditor::View *view);
    virtual void removeView(KTextEditor::View *view);
    virtual void addDocument(KTextEditor::Document* document);

private:
    QList<class KobbyPluginView*> m_views;
    Kobby::Connection* m_connection;
    bool m_isConnected;

public slots:
    void connected(Connection*);
    void documentUrlChanged(KTextEditor::Document*);
};

class KobbyPluginView : public QObject
{
  Q_OBJECT
  public:
    explicit KobbyPluginView(KTextEditor::View *view, Kobby::Connection* connection);
    ~KobbyPluginView();

    KTextEditor::View* view() const;

  public Q_SLOTS:
    void selectionChanged();
    void textInserted(KTextEditor::Document*,KTextEditor::Range);
    void textRemoved(KTextEditor::Document*,KTextEditor::Range);
    void connected(Kobby::Connection* connection);

  private:
    KTextEditor::View* m_view;
    Kobby::Connection* m_connection;
    QInfinity::BrowserModel* m_browserModel;
    QInfinity::NotePlugin* m_textPlugin;
    Kobby::DocumentBuilder* m_docBuilder;
};

#endif // _KOBBY_PLUGIN_H_

// kate: space-indent on; indent-width 4; replace-tabs on;
