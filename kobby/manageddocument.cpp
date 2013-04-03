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

#include "manageddocument.h"

#include <libqinfinity/session.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/textsession.h>
#include <libqinfinity/browseritemfactory.h>
#include <libqinfinity/xmlconnection.h>
#include <libqinfinity/xmppconnection.h>

#include <common/connection.h>

#include <QTimer>

#include "common/utils.h"

using namespace QInfinity;

ManagedDocument::ManagedDocument(KTextEditor::Document* document, BrowserModel* browserModel, NotePlugin* plugin, Kobby::Connection* connection, QObject* parent)
    : QObject(parent)
    , m_document(document)
    , m_browserModel(browserModel)
    , m_notePlugin(plugin)
    , m_subscribed(false)
    , m_textBuffer(0)
    , m_infDocument(0)
    , m_connection(connection)
{
    kDebug() << "now managing document" << document << document->url();
}

ManagedDocument::~ManagedDocument()
{
    unsubscribe();
}

QInfinity::Browser* ManagedDocument::browser() const
{
    Q_ASSERT(m_connection->xmppConnection() && "requested browser for an inactive connection!");
    for ( int i = 0; i < m_browserModel->rowCount(); i++ ) {
        ConnectionItem* item = dynamic_cast<ConnectionItem*>(m_browserModel->item(i));
        if ( item && item->connection() == static_cast<QInfinity::XmlConnection*>(m_connection->xmppConnection()) ) {
            return item->browser();
        }
    }
    Q_ASSERT(false && "no browser found for connection");
    return 0;
}

void ManagedDocument::unsubscribe()
{
    kDebug() << "should unsubscribe document";
    if ( m_infDocument ) {
        m_infDocument->leave();
        delete m_infDocument;
    }
}

bool ManagedDocument::isSubscribed() const
{
    return m_subscribed;
}

void ManagedDocument::subscribe()
{
    if ( m_document->url().protocol() != "inf" ) {
        return;
    }
    kDebug() << "beginning subscription for" << m_document->url();
    IterLookupHelper* helper = new IterLookupHelper(m_document->url().path(KUrl::RemoveTrailingSlash), browser());
    connect(helper, SIGNAL(done(QInfinity::BrowserIter)),
            this, SLOT(finishSubscription(QInfinity::BrowserIter)));
    helper->begin();
}

void ManagedDocument::subscriptionDone(QInfinity::BrowserIter iter, QPointer< QInfinity::SessionProxy > proxy)
{
    kDebug() << "subscription done, waiting for sync" << proxy->session()->status() << QInfinity::Session::Running;
    m_proxy = proxy;
//     QObject::connect(proxy->session(), SIGNAL(statusChanged()),
//                      this, SLOT(sessionStatusChanged()));
    QInfinity::TextSession* textSession = dynamic_cast<QInfinity::TextSession*>(proxy.data()->session().data());
    m_infDocument = new Kobby::InfTextDocument(proxy.data(), textSession,
                                               m_textBuffer, document()->documentName());
}

void ManagedDocument::sessionStatusChanged()
{
#warning TODO remove this function
    kDebug() << "session status changed to " << m_proxy->session()->status();
    if ( m_proxy->session()->status() != QInfinity::Session::Running ) {
        kDebug() << "not running, ignoring event";
        return;
    }
}

void ManagedDocument::finishSubscription(QInfinity::BrowserIter iter)
{
    // delete the lookup helper
    QObject::sender()->deleteLater();
    kDebug() << "finishing subscription with iter " << iter.path();
    QPointer< QInfinity::Browser > browser = iter.browser();
    QObject::connect(browser.data(), SIGNAL(subscribeSession(QInfinity::BrowserIter,QPointer<QInfinity::SessionProxy>)),
                     this, SLOT(subscriptionDone(QInfinity::BrowserIter,QPointer<QInfinity::SessionProxy>)));
    m_textBuffer = new Kobby::KDocumentTextBuffer(document(), "utf-8");
    browser->subscribeSession(iter, m_notePlugin, m_textBuffer);
}

ManagedDocument* ManagedDocumentList::findDocument(KTextEditor::Document* document) const
{
    ManagedDocument* found = 0;
    foreach ( ManagedDocument* doc, *this ) {
        if ( doc->document() == document ) {
            found = doc;
        }
    }
    return found;
}

bool ManagedDocumentList::isManaged(KTextEditor::Document* document) const
{
    return findDocument(document) != 0;
}