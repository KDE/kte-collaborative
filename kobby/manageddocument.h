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

#ifndef KOBBY_MANAGEDDOCUMENT_H
#define KOBBY_MANAGEDDOCUMENT_H

#include <QObject>

#include <KTextEditor/Document>

#include <libqinfinity/browsermodel.h>
#include <libqinfinity/sessionproxy.h>
#include <libqinfinity/browseriter.h>
#include <libqinfinity/session.h>

#include <common/document.h>
#include <common/connection.h>

using Kobby::Connection;
using Kobby::Document;

class ManagedDocument : public QObject {
Q_OBJECT
public:
    ManagedDocument(KTextEditor::Document* document, QInfinity::BrowserModel* browserModel,
                    QInfinity::NotePlugin* plugin, Kobby::Connection* connection, QObject* parent = 0);
    virtual ~ManagedDocument();
    void subscribe();
    void unsubscribe();
    bool isSubscribed() const;
    QInfinity::Session::Status sessionStatus() const;
    inline KTextEditor::Document* document() const {
        return m_document;
    };
    inline Kobby::InfTextDocument* infTextDocument() const {
        return m_infDocument;
    };
    inline Kobby::KDocumentTextBuffer* textBuffer() const {
        return m_textBuffer;
    };
    // Returns the Browser for the document's connection
    QInfinity::Browser* browser() const;
    // Returns the Connection for this document
    Kobby::Connection* connection() const;
    // Returns the user table for this document
    QInfinity::UserTable* userTable() const;

public slots:
    void finishSubscription(QInfinity::BrowserIter iter);
    void subscriptionDone(QInfinity::BrowserIter, QPointer<QInfinity::SessionProxy>);
    void sessionStatusChanged();
    void synchronizationComplete(Document*);
    void disconnected(Connection*);

signals:
    void documentReady(ManagedDocument* document);
    void synchronizationBegins(ManagedDocument* document);

private:
    Kobby::KDocumentTextBuffer* m_textBuffer;
    KTextEditor::Document* m_document;
    QInfinity::BrowserModel* m_browserModel;
    QInfinity::NotePlugin* m_notePlugin;
    Kobby::Connection* m_connection;
    bool m_subscribed;
    QPointer< QInfinity::SessionProxy > m_proxy;
    Kobby::InfTextDocument* m_infDocument;
    // id of the browser iter for this document, for checking whether signals are meant for it
    int m_iterId;
    // status of this session (synchronizing, running etc)
    QInfinity::Session::Status m_sessionStatus;
};

typedef QMap<KTextEditor::Document*, ManagedDocument*> ManagedDocumentList;

#endif
