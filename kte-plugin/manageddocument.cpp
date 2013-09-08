/* This file is part of the Kobby plugin
 * Copyright (C) 2013 Sven Brauch <svenbrauch@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "manageddocument.h"

#include "documentchangetracker.h"
#include "common/connection.h"
#include "common/utils.h"
#include <common/noteplugin.h>

#include <libqinfinity/session.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/textsession.h>
#include <libqinfinity/browseritemfactory.h>
#include <libqinfinity/xmlconnection.h>
#include <libqinfinity/xmppconnection.h>
#include <libqinfinity/usertable.h>

#include <KMessageBox>
#include <KLocalizedString>
#include <KIO/Job>
#include <KDialog>
#include <KPushButton>

#include <QTimer>
#include <QFile>
#include <QTemporaryFile>
#include <QLabel>

using namespace QInfinity;

ManagedDocument::ManagedDocument(KTextEditor::Document* document, BrowserModel* browserModel, NotePlugin* plugin, Kobby::Connection* connection, QObject* parent)
    : QObject(parent)
    , m_textBuffer(0)
    , m_document(document)
    , m_browserModel(browserModel)
    , m_notePlugin(plugin)
    , m_connection(connection)
    , m_subscribed(false)
    , m_ready(false)
    , m_infDocument(0)
    , m_iterId(0)
    , m_sessionStatus(QInfinity::Session::Closed)
    , m_localSavePath()
    , m_changeTracker(new DocumentChangeTracker(this))
    , m_connectionRetries(0)
{
    kDebug() << "now managing document" << document << document->url();
    // A document must not be edited before it is connected, since changes done will
    // not be synchronized with the server; this causes inconsistency.
    document->setReadWrite(false);
    connect(m_connection, SIGNAL(disconnected(Connection*)),
            this, SLOT(disconnected(Connection*)));
}

ManagedDocument::~ManagedDocument()
{
    unsubscribe();
}

bool ManagedDocument::saveCopy() const
{
    if ( localSavePath().isEmpty() ) {
        kDebug() << "invalid save url";
        return false;
    }
    QFile f(localSavePath());
    // TODO can we do this less manually?
    QByteArray contents = m_document->text().toUtf8();
    if ( ! f.open(QIODevice::WriteOnly) ) {
        kDebug() << "failed to open" << localSavePath() << "for writing";
        return false;
    }
    if ( f.write(contents) != contents.size() ) {
        kDebug() << "failed to write" << contents.size() << "bytes to" << localSavePath();
        return false;
    }
    m_document->setModified(false);
    return true;
}

QInfinity::Browser* ManagedDocument::browser() const
{
    if ( ! m_connection->xmppConnection() ) {
        return 0;
    }
    for ( int i = 0; i < m_browserModel->rowCount(); i++ ) {
        ConnectionItem* item = dynamic_cast<ConnectionItem*>(m_browserModel->item(i));
        if ( item && item->connection() == static_cast<QInfinity::XmlConnection*>(m_connection->xmppConnection()) ) {
            return item->browser();
        }
    }
    return 0;
}

Kobby::Connection* ManagedDocument::connection() const
{
    return m_connection;
}

void ManagedDocument::unsubscribe()
{
    kDebug() << "should unsubscribe document";
    m_ready = false;
    if ( m_infDocument ) {
        m_infDocument->leave();
        m_infDocument->deleteLater();
        m_infDocument = 0;
        m_textBuffer->shutdown();
        m_textBuffer = 0;
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
    m_subscribed = true;
    kDebug() << "beginning subscription for" << m_document->url();
    IterLookupHelper* helper = new IterLookupHelper(m_document->url().path(KUrl::RemoveTrailingSlash), browser());
    connect(helper, SIGNAL(done(QInfinity::BrowserIter)),
            this, SLOT(finishSubscription(QInfinity::BrowserIter)));
    connect(helper, SIGNAL(failed()),
            this, SLOT(lookupFailed()));
    helper->begin();
}

void ManagedDocument::lookupFailed()
{
    // If the lookup fails, try again after a few seconds.
    // TODO this is a workaround for race conditions in the "file added" notifications, fix it!
    if ( m_connectionRetries < 5 ) {
        connect(browser(), SIGNAL(nodeAdded(BrowserIter)), SLOT(subscribe()));
        QTimer::singleShot(3000, this, SLOT(subscribe()));
    }
    else {
        unsubscribe();
        KMessageBox::error(document()->widget(),
                           i18n("Failed to open file %1, make sure it exists.", document()->url().url()));
    }
    m_connectionRetries += 1;
}

void ManagedDocument::subscriptionDone(QInfinity::BrowserIter iter, QPointer< QInfinity::SessionProxy > proxy)
{
    if ( iter.id() != m_iterId ) {
        kDebug() << "subscription done, but not for this document";
        return;
    }
    kDebug() << "subscription done, waiting for sync" << proxy->session()->status() << QInfinity::Session::Running;
    m_proxy = proxy;
    QObject::connect(proxy->session(), SIGNAL(statusChanged()),
                     this, SLOT(sessionStatusChanged()));
    QObject::connect(proxy->session(), SIGNAL(progress(double)),
                     this, SIGNAL(synchroinzationProgress(double)));
    QInfinity::TextSession* textSession = dynamic_cast<QInfinity::TextSession*>(proxy->session().data());
    m_infDocument = new Kobby::InfTextDocument(proxy.data(), textSession,
                                               m_textBuffer, document()->documentName());
    connect(m_infDocument, SIGNAL(fatalError(Document*,QString)),
            this, SLOT(unrecoverableError(Document*,QString)));
    connect(m_infDocument, SIGNAL(loadingComplete(Document*)),
            this, SLOT(synchronizationComplete(Document*)));
    connect(m_infDocument, SIGNAL(loadStateChanged(Document*,Document::LoadState)),
            this, SIGNAL(loadStateChanged(Document*,Document::LoadState)));
    m_textBuffer->setSession(proxy->session());
    emit synchronizationBegins(this);
}

void ManagedDocument::unrecoverableError(Document* document, QString error)
{
    Q_ASSERT(document == m_infDocument);
    if ( document->kDocument() ) {
        QTemporaryFile file;
        file.setAutoRemove(false);
        file.open();
        file.close();
        document->kDocument()->saveAs(KUrl(file.fileName()));
        if ( ! error.isEmpty() ) {
            // We must not use exec() here (so no KMessageBox!) or we will run into
            // nested-event-loop-network-code trouble.
            KDialog* dlg = new KDialog();
            dlg->setMainWidget(new QLabel(i18n("Error opening document: %1", error)));
            dlg->setButtons(KDialog::Cancel);
            dlg->button(KDialog::Cancel)->setText(i18n("Disconnect"));
            dlg->setAttribute(Qt::WA_DeleteOnClose);
            dlg->show();
        }
    }
}

void ManagedDocument::synchronizationComplete(Kobby::Document* /*document*/)
{
    // Only after the connection has been established and synchronization is finished,
    // the user is allowed to edit the document.
    document()->setReadWrite(true);
    m_ready = true;
    emit documentReady(this);
}

bool ManagedDocument::isReady() const
{
    return m_ready;
}

void ManagedDocument::disconnected(Kobby::Connection* )
{
    // If a connection for a document gets disconnected, it should be
    // set to read-only, to prevent a user from further editing the document
    // without saving it somewhere.
    document()->setReadWrite(false);
    m_ready = false;
}

UserTable* ManagedDocument::userTable() const
{
    return m_proxy->session()->userTable().data();
}

Session::Status ManagedDocument::sessionStatus() const
{
    return m_sessionStatus;
}

void ManagedDocument::sessionStatusChanged()
{
    m_sessionStatus = m_proxy->session()->status();
    kDebug() << "session status changed to " << m_proxy->session()->status() << "on" << document()->url();
}

void ManagedDocument::finishSubscription(QInfinity::BrowserIter iter)
{
    // delete the lookup helper
    QObject::sender()->deleteLater();
    kDebug() << "finishing subscription with iter " << iter.path();
    QPointer< QInfinity::Browser > browser = iter.browser();
    QObject::connect(browser.data(), SIGNAL(subscribeSession(QInfinity::BrowserIter,QPointer<QInfinity::SessionProxy>)),
                     this, SLOT(subscriptionDone(QInfinity::BrowserIter,QPointer<QInfinity::SessionProxy>)), Qt::UniqueConnection);
    m_textBuffer = new Kobby::KDocumentTextBuffer(document(), "utf-8", static_cast<Kobby::NotePlugin*>(m_notePlugin));
    kDebug() << "created text buffer";
    m_iterId = iter.id();
    browser->subscribeSession(iter, m_notePlugin, m_textBuffer);
}

#include "manageddocument.moc"
