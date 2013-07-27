/***************************************************************************
 *   Copyright (C) 2013 by Sven Brauch <svenbrauch@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef TUBESTEST_H
#define TUBESTEST_H

#include <TelepathyQt/Types>
#include <TelepathyQt/StreamTubeChannel>
#include <TelepathyQt/IncomingStreamTubeChannel>
#include <TelepathyQt/OutgoingStreamTubeChannel>
#include <TelepathyQt/StreamTubeClient>
#include <TelepathyQt/StreamTubeServer>
#include <TelepathyQt/AccountManager>
#include <TelepathyQt/Account>
#include <TelepathyQt/ClientRegistrar>
#include <TelepathyQt/AbstractClientObserver>
#include <TelepathyQt/ChannelClassSpecList>
#include <KTp/telepathy-handler-application.h>

#include <KJob>
#include <KUrl>
#include <KDebug>
#include <QTcpSocket>

#include "inftube_export.h"

namespace Tp {
    class StreamTubeClient;
}

class ServerManager;

typedef QList<KUrl> DocumentList;

inline Tp::ChannelClassSpecList channelClassList()
{
    return Tp::ChannelClassSpecList() << Tp::ChannelClassSpec::incomingStreamTube("infinity");
}

/**
 * This class defines the API for routing infinity traffic through a TP tube
 * which is common to both the receiving an the offering side.
 */
class INFTUBE_EXPORT InfTubeBase : public QObject {
Q_OBJECT
public:
    explicit InfTubeBase(QObject* parent = 0);
    virtual ~InfTubeBase();

    enum ConnectionStatus {
        StatusConnected,
        StatusConnecting,
        StatusDisconnected
    };

    /**
     * @brief Get the local port this tube ends in.
     * @return unsigned int the port
     */
    unsigned int localPort() const;

    /**
     * @brief Get the URL which points to the infinote server.
     * @return KUrl the URL, something like inf://localhost:foo
     */
    KUrl localUrl() const;

    /**
     * @brief Set a nickname with all "bad" characters properly escaped.
     */
    void setNicknameFromAccount(const Tp::AccountPtr& account);

    /**
     * @brief Get the nickname used in documents for this tube by default
     */
    const QString& nickname() const;

    const ServerManager* connectionManager() const;

signals:
    /**
     * @brief Emitted when the connection has been established
     */
    void connected(InfTubeBase* self);

protected:
    unsigned int m_port;
    QString m_nickname;
};

/**
 * @brief This class implements InfTubeBase and is used on the receiving end.
 */
class INFTUBE_EXPORT InfTubeClient : public InfTubeBase {
Q_OBJECT
public:
    virtual ~InfTubeClient();

    /**
     * @brief Begin waiting for incoming connections.
     */
    void listen();

signals:
    /**
     * @brief Emitted as soon as the tube is connected.
     */
    void connected();

private:
    Tp::StreamTubeClientPtr m_tubeClient;

public slots:
    /**
     * @brief Called when a tube gets accepted.
     */
    void tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr);
};

// This class is for requesting a new tube
class INFTUBE_EXPORT InfTubeRequester : public InfTubeBase {
Q_OBJECT
public:
    explicit InfTubeRequester(QObject* parent = 0);
    /**
     * @brief Initiate editing a list of documents with the given contacts.
     *
     * @param contacts The list of contacts to edit documents with
     * @param initialDocuments The documents all contacts should have opened initially
     * @return bool true if the request was successful
     */
    bool offer(const Tp::AccountPtr& account, const Tp::Contacts& contacts, const DocumentList& documents);

    /**
     * @brief Offer the given documents to the given contact
     *
     * @param account The account to use to create the offer
     * @param contact The contact to share the documents with
     * @param documents A list of documents to share. Must not be empty.
     * @return bool true on success
     */
    bool offer(const Tp::AccountPtr& account, const Tp::ContactPtr& contact, const DocumentList& documents);

    /**
     * @brief Offer the given documents to an existing (!) chatroom.
     *
     * @param account The acconut to use to create the offer
     * @param chatroom The chatroom to offer the tube to
     * @param documents A list of documents to share initially. Must not be empty.
     * @return bool true on success
     */
    bool offer(const Tp::AccountPtr& account, const QString& chatroom, const DocumentList& documents);

private:
    /**
     * @brief Create a QVariantMap containing a list of documents to transmit with the tube ofer
     *
     * @param documents The doucments which you want to be opened initially
     * @return const QVariantMap A QVariantMap containing the document list, suitable to be used in a tube request
     */
    const QVariantMap createHints(const DocumentList& documents) const;

    bool createRequest(const Tp::AccountPtr account, const DocumentList documents, QVariantMap requestBase);
public slots:
    void onTubeRequestReady(Tp::PendingOperation*);
    void onTubeReady(Tp::PendingOperation*);
};

// This class is for handling a requested tube. It will handle the channel request,
// and set some parameters (such as the existing local endpoint if the channel is
// already open), before handing the channel back to the requester.
// It is supposed to exist in a separate process (see servertubehandler.cpp)
class INFTUBE_EXPORT InfTubeServer : public InfTubeBase {
Q_OBJECT
public:
    InfTubeServer(QObject* parent = 0);
    virtual ~InfTubeServer();

    /**
     * @brief Create a StreamTubeServer instance and register it on dbus, to start listening for channel requests.
     */
    void registerHandler();

    QString serverDirectory(unsigned short port) const;

public slots:
    void tubeRequested(Tp::AccountPtr,Tp::OutgoingStreamTubeChannelPtr,QDateTime,Tp::ChannelRequestHints);

private:
    QList<Tp::StreamTubeChannelPtr> m_channels;
    Tp::StreamTubeServerPtr m_tubeServer;
    QList<QProcess*> m_serverProcesses;
    bool m_hasCreatedChannel;

    /**
     * @brief Starts infinoted and provides the port
     *
     * @return bool true if successful, else false.
     */
    bool startInfinoted(unsigned short* port);
};

/**
 * @brief Container for all tubes offered over time and for the Tp objects which are only needed once
 */
class ServerManager : public QObject {
Q_OBJECT
public:
    explicit ServerManager(QObject* parent = 0);
    static ServerManager* instance();

    void add(InfTubeServer* server);

    Tp::AccountManagerPtr accountManager;

private:
    QList<InfTubeServer*> m_serverProcesses;
    void initialize();

private slots:
    void shutdown();
};

#endif
