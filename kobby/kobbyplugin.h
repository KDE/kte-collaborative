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
#include <QStack>
#include <kjob.h>
#include <kurl.h>
#include <kio/job.h>
#include "editor/connection.h"
#include "editor/documentbuilder.h"
#include <libqinfinity/qgsignal.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browseriter.h>
#include <libqinfinity/browser.h>

using namespace Kobby;

class ManagedDocument : public QObject {
Q_OBJECT
public:
    ManagedDocument(KTextEditor::Document* document, QInfinity::BrowserModel* model);
    void subscribe();
    void unsubscribe();
    bool isSubscribed();
public slots:
    void finishSubscription(QInfinity::BrowserIter iter);
private:
    KTextEditor::Document* m_document;
    QInfinity::BrowserModel* m_browserModel;
    bool m_subscribed;
};

class IterLookupHelper : public QObject {
Q_OBJECT
public:
    IterLookupHelper(QString lookupPath, QInfinity::Browser* browser)
        : m_browser(browser)
    {
        m_remainingDirs << lookupPath.split('/').toVector();
    };
    static void finished_cb( InfcNodeRequest* request,
                             const InfcBrowserIter* iter,
                             void* user_data )
    {
        qDebug() << "iter explore finished";
        static_cast<IterLookupHelper*>(user_data)->directoryExplored(iter);
    }

    void begin() {
        explore(QInfinity::BrowserIter(*m_browser));
    };

signals:
    void done(QInfinity::BrowserIter found);
    void failed();

protected:
    void directoryExplored(const InfcBrowserIter* iter_) {
        QInfinity::BrowserIter iter(iter_, INFC_BROWSER(m_browser->gobject()));
        QString findEntry = m_remainingDirs.pop();
        // ... find matching item
        // found = ...
        if ( m_remainingDirs.isEmpty() ) {
            // no directories remain
//             emit done(found);
        }
        else {
//             explore(found);
        }
    };

    void explore(QInfinity::BrowserIter directory) {
        if ( ! directory.isExplored() ) {
            kDebug() << "exploring iter";
            InfcExploreRequest* request = directory.explore();
            g_signal_connect_after(request, "finished",
                                   G_CALLBACK(IterLookupHelper::finished_cb), (void*) this);
        }
    };

    QStack<QString> m_remainingDirs;
    QInfinity::Browser* m_browser;
};

class KobbyPluginView;

class KobbyPlugin : public KTextEditor::Plugin {
Q_OBJECT
public:
    explicit KobbyPlugin( QObject *parent = 0,
                        const QVariantList &args = QVariantList() );
    virtual ~KobbyPlugin();

    virtual void addView(KTextEditor::View *view);
    virtual void removeView(KTextEditor::View *view);
    virtual void addDocument(KTextEditor::Document* document);

    void subscribeNewDocuments();

private:
    QList<KobbyPluginView*> m_views;
    QList<ManagedDocument*> m_managedDocuments;
    bool m_isConnected;
    Kobby::Connection* m_connection;
    QInfinity::BrowserModel* m_browserModel;
    QInfinity::NotePlugin* m_textPlugin;
    Kobby::DocumentBuilder* m_docBuilder;

public slots:
    void connected(Connection*);
    void documentUrlChanged(KTextEditor::Document*);
    void textInserted(KTextEditor::Document*,KTextEditor::Range);
    void textRemoved(KTextEditor::Document*,KTextEditor::Range);
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

  private:
    KTextEditor::View* m_view;
};

#endif // _KOBBY_PLUGIN_H_

// kate: space-indent on; indent-width 4; replace-tabs on;
