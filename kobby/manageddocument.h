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

#include <common/document.h>
#include <common/connection.h>

class ManagedDocument : public QObject {
Q_OBJECT
public:
    ManagedDocument(KTextEditor::Document* document, QInfinity::BrowserModel* browserModel,
                    QInfinity::NotePlugin* plugin, Kobby::Connection* connection, QObject* parent = 0);
    virtual ~ManagedDocument();
    void subscribe();
    void unsubscribe();
    bool isSubscribed() const;
    inline KTextEditor::Document* document() const {
        return m_document;
    };
    // Returns the Browser for the document's connection
    QInfinity::Browser* browser() const;

public slots:
    void finishSubscription(QInfinity::BrowserIter iter);
    void subscriptionDone(QInfinity::BrowserIter, QPointer<QInfinity::SessionProxy>);
    void sessionStatusChanged();

private:
    Kobby::KDocumentTextBuffer* m_textBuffer;
    KTextEditor::Document* m_document;
    QInfinity::BrowserModel* m_browserModel;
    QInfinity::NotePlugin* m_notePlugin;
    Kobby::Connection* m_connection;
    bool m_subscribed;
    QPointer< QInfinity::SessionProxy > m_proxy;
    Kobby::InfTextDocument* m_infDocument;
};

class ManagedDocumentList : public QList<ManagedDocument*> {
public:
    bool isManaged(KTextEditor::Document* document) const;
    ManagedDocument* findDocument(KTextEditor::Document* document) const;
};

#endif
