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
#include "common/connection.h"
#include "common/documentbuilder.h"
#include <libqinfinity/qgsignal.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browseriter.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/textsession.h>

using namespace Kobby;

class ManagedDocument : public QObject {
Q_OBJECT
public:
    ManagedDocument(KTextEditor::Document* document, QInfinity::BrowserModel* model);
    virtual ~ManagedDocument();
    void subscribe();
    void unsubscribe();
    bool isSubscribed();
    KTextEditor::Document* document() const {
        return m_document;
    };
    KDocumentTextBuffer* m_textBuffer;
public slots:
    void finishSubscription(QInfinity::BrowserIter iter);
    void subscriptionDone(QInfinity::BrowserIter,QPointer<QInfinity::SessionProxy>);
    void userJoinCompleted(QPointer<QInfinity::User>);
    void sessionStatusChanged();
    void joinUser();
private:
    KTextEditor::Document* m_document;
    QInfinity::BrowserModel* m_browserModel;
    bool m_subscribed;
    QPointer< QInfinity::SessionProxy > m_proxy;
};

class IterLookupHelper : public QObject {
Q_OBJECT
public:
    IterLookupHelper(QString lookupPath, QInfinity::Browser* browser)
        : m_browser(browser)
        , m_currentIter(*m_browser)
    {
        // remove starting slash
        if ( lookupPath.startsWith('/') ) {
            lookupPath = lookupPath.mid(1);
        }
        m_remainingComponents << lookupPath.split('/').toVector();
        kDebug() << "finding iter for" << m_remainingComponents;
    };
    static void finished_cb( InfcNodeRequest* request,
                             void* user_data )
    {
        static_cast<IterLookupHelper*>(user_data)->directoryExplored();
    }

    void begin() {
        explore(m_currentIter);
    };

signals:
    void done(QInfinity::BrowserIter found);
    void failed();

protected:
    void directoryExplored() {
        kDebug() << "directory explored";
        QString findEntry = m_remainingComponents.pop();
        bool hasChildren = m_currentIter.child();
        if ( ! hasChildren ) {
            emit failed();
            return;
        }

        bool found = false;
        do {
            kDebug() << m_currentIter.name();
            if ( m_currentIter.name() == findEntry ) {
                found = true;
                break;
            }
        } while ( m_currentIter.next() );

        if ( m_remainingComponents.isEmpty() ) {
            // no directories remain
            emit done(m_currentIter);
        }
        else if ( found ) {
            explore(m_currentIter);
        }
        kWarning() << "explore failed!";
        emit failed();
    };

    void explore(QInfinity::BrowserIter directory) {
        if ( ! directory.isExplored() ) {
            kDebug() << "exploring iter";
            InfcExploreRequest* request = directory.explore();
            m_currentIter = directory;
            g_signal_connect_after(request, "finished",
                                   G_CALLBACK(IterLookupHelper::finished_cb), (void*) this);
        }
    };

    QStack<QString> m_remainingComponents;
    QInfinity::Browser* m_browser;
    QInfinity::BrowserIter m_currentIter;
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
    virtual void removeDocument(KTextEditor::Document* document);

    void subscribeNewDocuments();

private:
    QList<KobbyPluginView*> m_views;
    QList<ManagedDocument*> m_managedDocuments;
    bool m_isConnected;
    bool m_browserReady;
    Kobby::Connection* m_connection;
    QInfinity::BrowserModel* m_browserModel;
    QInfinity::NotePlugin* m_textPlugin;
    Kobby::DocumentBuilder* m_docBuilder;
    QInfinity::CommunicationManager* m_communicationManager;
    QInfinity::TextSession* m_session;

public slots:
    // This is called when the underlying connection is established.
    // It does not mean that the browser is ready to be used.
    void connected(Connection*);
    // This is called when the browser is ready.
    void browserConnected(const QInfinity::Browser*);
    void connectionPrepared();
    void documentUrlChanged(KTextEditor::Document*);
    void textInserted(KTextEditor::Document*,KTextEditor::Range);
    void textRemoved(KTextEditor::Document*,KTextEditor::Range);
    void userJoinCompleted(QPointer<QInfinity::User>);

    friend class ManagedDocument;
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
