/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "documentbuilder.h"
#include "document.h"
#include "kobbysettings.h"

#include <glib/gerror.h>

#include <libqinfinity/session.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/browseriter.h>
#include <libqinfinity/browseritemfactory.h>
#include <libqinfinity/userrequest.h>
#include <libqinfinity/session.h>
#include <libqinfinity/textsession.h>

#include <KTextEditor/Editor>
#include <KTextEditor/Document>
#include <KLocalizedString>
#include <KMessageBox>
#include <KUrl>
#include <KDebug>

#include "documentbuilder.moc"

namespace Kobby
{

DocumentBuilder::DocumentBuilder( KTextEditor::Editor &editor,
    QInfinity::BrowserModel &browserModel,
    QObject *parent )
    : QObject( parent )
    , editor( &editor )
    , m_browserModel( &browserModel )
{
    QInfinity::Browser *itr;
    foreach( itr, browserModel.browsers() )
    {
        slotBrowserAdded( *itr );
    }
    connect( m_browserModel, SIGNAL(browserAdded( QInfinity::Browser& )),
        this, SLOT(slotBrowserAdded( QInfinity::Browser& )) );
}

DocumentBuilder::~DocumentBuilder()
{
}

KDocumentTextBuffer *DocumentBuilder::createKDocumentTextBuffer(
    const QString &encoding )
{
    KDocumentTextBuffer *doc = new KDocumentTextBuffer( *editor->createDocument( this ),
        encoding, this );
    return doc;
}

void DocumentBuilder::openBlank()
{
    Document *doc = new Document( *editor->createDocument( this ) );
    emit( documentCreated( *doc ) );
}

void DocumentBuilder::openInfDocmuent( const QModelIndex &index )
{
    QStandardItem *stdItem = m_browserModel->itemFromIndex( index );
    QInfinity::NodeItem *nodeItem;

    if( stdItem->type() != QInfinity::BrowserItemFactory::NodeItem )
    {
        kDebug() << "Cannot open non-node item.";
        return;
    }

    nodeItem = dynamic_cast<QInfinity::NodeItem*>(stdItem);
    QInfinity::BrowserIter nodeItr = nodeItem->iter();
    nodeItr.browser()->subscribeSession( nodeItr );
}

void DocumentBuilder::openUrl( const KUrl &url )
{
    KTextEditor::Document *kdoc = editor->createDocument( this );
    Document *doc;
    kdoc->openUrl( url );
    doc = new Document( *kdoc );
    emit( documentCreated( *doc ) );
}

void DocumentBuilder::sessionSubscribed( const QInfinity::BrowserIter &iter,
    QPointer<QInfinity::SessionProxy> sessProxy )
{
    QInfinity::Session *session = sessProxy->session();
    if( session->status() == QInfinity::Session::Running )
        sessionSynchronized( session );
    else if( session->status() == QInfinity::Session::Synchronizing )
    {
        sessionToProxy[session] = sessProxy;
        connect( session, SIGNAL(synchronizationComplete()),
            this, SLOT(slotSessionSynchronized()) );
        connect( session, SIGNAL(synchronizationFailed( GError* )),
            this, SLOT(slotSessionSynchronizationFailed( GError* )) );
    }

    QInfinity::Buffer *infBuff = session->buffer();
    KDocumentTextBuffer *kbuff = dynamic_cast<KDocumentTextBuffer*>(infBuff);
    QInfinity::BrowserIter bi( iter );
    kbuff->setName( bi.name() );
    emit(documentCreated(*kbuff));
}

void DocumentBuilder::slotSessionSynchronized()
{
    QInfinity::Session *session = dynamic_cast<QInfinity::Session*>(sender());
    sessionSynchronized( session );
}

void DocumentBuilder::slotSessionSynchronizationFailed( GError *error )
{
    QString errorMsg = i18n("Syncronization Failed: ");
    errorMsg.append( error->message );

    KMessageBox::error( 0, i18n("Syncronization failed"), errorMsg );
}

void DocumentBuilder::sessionSynchronized( QInfinity::Session *session )
{
    QInfinity::Buffer *infBuff = session->buffer();
    KDocumentTextBuffer *kbuff = dynamic_cast<KDocumentTextBuffer*>(infBuff);
    if( !kbuff )
    {
        kDebug() << "Non kobby buffer, cannot join.";
        return;
    }
    QInfinity::UserRequest *req = QInfinity::TextSession::joinUser( sessionToProxy[session],
        KobbySettings::nickName(),
        0 );
    connect( req, SIGNAL(finished(QPointer<QInfinity::User>)),
        kbuff, SLOT(setUser(QPointer<QInfinity::User>)) );
    connect( req, SIGNAL(failed(GError*)),
        kbuff, SLOT(joinFailed(GError*)) );
}

void DocumentBuilder::slotBrowserAdded( QInfinity::Browser &browser )
{
    connect( &browser, SIGNAL(subscribeSession(const QInfinity::BrowserIter&,
            QPointer<QInfinity::SessionProxy>)),
        this, SLOT(sessionSubscribed(QInfinity::BrowserIter,
            QPointer<QInfinity::SessionProxy>)) );
}

}

