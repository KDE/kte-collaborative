/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 * Copyright 2009  Ryan Kavanagh <ryanakca@kubuntu.org>
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
#include "documentmodel.h"
#include "kobbysettings.h"

#include <glib.h>

#include <libqinfinity/session.h>
#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browser.h>
#include <libqinfinity/browseriter.h>
#include <libqinfinity/browseritemfactory.h>
#include <libqinfinity/userrequest.h>
#include <libqinfinity/session.h>
#include <libqinfinity/textsession.h>

#include <libinfinity/client/infc-session-proxy.h>

#include <KTextEditor/Editor>
#include <KTextEditor/Document>
#include <KLocalizedString>
#include <KMessageBox>
#include <KUrl>
#include <KDebug>

#include "documentbuilder.moc"

namespace Kobby
{

DocumentBuilder::DocumentBuilder( QInfinity::BrowserModel &browserModel, QObject *parent )
    : QObject( parent )
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
    KDocumentTextBuffer *doc = new KDocumentTextBuffer( editor->createDocument( this ),
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
    kDebug() << index.parent().isValid() << index.parent().row() << index.parent().column();

    if( stdItem->type() != QInfinity::BrowserItemFactory::NodeItem )
    {
        kDebug() << "Cannot open non-node item" << index.row() << index.column() << stdItem->type() << index.isValid();
        return;
    }
    kDebug() << "item ok";
    nodeItem = dynamic_cast<QInfinity::NodeItem*>(stdItem);
    if(nodeItem->isDirectory())
        return;

    Document *doc = DocumentModel::instance()->documentFromNodeItem(*nodeItem);
    if( doc )
        emit(documentCreated( *doc ));
    else
    {
        QInfinity::BrowserIter nodeItr = nodeItem->iter();

        // Document view  will be added from session subscribe_begin emission
        nodeItr.browser()->subscribeSession( nodeItr );
    }
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
    QInfinity::TextSession *textSession = dynamic_cast<QInfinity::TextSession*>(sessProxy->session().data());
    
    if( !textSession || !INF_TEXT_IS_SESSION(textSession->gobject()) )
    {
        kDebug() << i18n("Session is not an InfText session.  This usually means " \
            "the connection is not using the kobby plugin.");
        return;
    }

    QInfinity::Buffer *infBuff = textSession->buffer();
    KDocumentTextBuffer *kbuff = dynamic_cast<KDocumentTextBuffer*>(infBuff);
    if( !kbuff )
    {
        kDebug() << i18n("Could not retrieve Kobby buffer from session.  This " \
            "usually means the connection is not using the kobby plugin.");
        return;
    }
    QInfinity::BrowserIter titr = iter;
    InfTextDocument *infDoc = new InfTextDocument( *sessProxy, *textSession, *kbuff, titr.name() );
    emit(documentCreated( *infDoc, &iter ));
}

void DocumentBuilder::slotBrowserAdded( QInfinity::Browser &browser )
{
    connect( &browser, SIGNAL(subscribeSession(const QInfinity::BrowserIter&,
            QPointer<QInfinity::SessionProxy>)),
        this, SLOT(sessionSubscribed(QInfinity::BrowserIter,
            QPointer<QInfinity::SessionProxy>)) );
}

}
