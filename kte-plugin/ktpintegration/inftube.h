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

#include <telepathy-qt4/TelepathyQt/Types>
#include <telepathy-qt4/TelepathyQt/StreamTubeChannel>
#include <telepathy-qt4/TelepathyQt/IncomingStreamTubeChannel>
#include <telepathy-qt4/TelepathyQt/OutgoingStreamTubeChannel>
#include <telepathy-qt4/TelepathyQt/StreamTubeClient>
#include <telepathy-qt4/TelepathyQt/StreamTubeServer>
#include <telepathy-qt4/TelepathyQt/AccountManager>
#include <telepathy-qt4/TelepathyQt/Account>
#include <telepathy-qt4/TelepathyQt/ClientRegistrar>
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

typedef QList<Tp::ContactPtr> ContactList;
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
public:
    virtual ~InfTubeBase() { };

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
    QTcpSocket* m_socket;

public slots:
    /**
     * @brief Called when a tube gets accepted.
     */
    void tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr);
};

/**
 * @brief This class implements InfTubeBase and can be used to offer some documents to a contact.
 */
class INFTUBE_EXPORT InfTubeServer : public InfTubeBase {
Q_OBJECT
public:
    InfTubeServer(QObject* parent = 0);
    virtual ~InfTubeServer();

    /**
     * @brief Initiate editing a list of documents with the given contacts.
     *
     * @param contacts The list of contacts to edit documents with
     * @param initialDocuments The documents all contacts should have opened initially
     * @return bool true if the request was successful
     */
    bool offer(Tp::AccountPtr account, const ContactList& contact, const DocumentList& documents);

    /**
     * @brief Offer the given documents to the given contact
     *
     * @param account The account to use to create the offer
     * @param contact The contact to share the documents with
     * @param documents A list of documents to share. Must not be empty.
     * @return bool true on success
     */
    bool offer(Tp::AccountPtr account, const Tp::ContactPtr contact, const DocumentList& documents);

    /**
     * @brief Offer the given documents to an existing (!) chatroom.
     *
     * @param account The acconut to use to create the offer
     * @param chatroom The chatroom to offer the tube to
     * @param documents A list of documents to share initially. Must not be empty.
     * @return bool true on success
     */
    bool offer(Tp::AccountPtr account, const QString& chatroom, const DocumentList& documents);

signals:
    /**
     * @brief Emitted when the connection has been established
     */
    void connected(InfTubeBase* self);

    /**
     * @brief Emitted when a file was copied to the server and is ready for editing
     *
     * @param url The inf:// URL to be used for editing this document, complete with nick name.
     */
    void fileCopiedToServer(const KUrl& url);

private slots:
    /**
     * @brief Invoked when creation of a tube finishes.
     */
    void onCreateTubeFinished(Tp::PendingOperation*);

    /**
     * @brief Invoked when a job copying an initial file to the server finishes
     */
    void jobFinished(KJob*);

private:
    Tp::StreamTubeServerPtr m_tubeServer;
    QProcess* m_serverProcess;

    /**
     * @brief Starts infinoted on a free port (avilable through port())
     *
     * @return bool true if successful, else false.
     */
    bool startInfinoted();

    /**
     * @brief Gets the directory to be used as the root directory of infinoted
     */
    const QString serverDirectory() const;

    /**
     * @brief Service name for the tube server to use on dbus.
     */
    const QString serviceName() const;

    /**
     * @brief Create a QVariantMap containing a list of documents to transmit with the tube ofer
     *
     * @param documents The doucments which you want to be opened initially
     * @return const QVariantMap A QVariantMap containing the document list, suitable to be used in a tube request
     */
    const QVariantMap createHints(const DocumentList& documents) const;

    bool proceed(const Tp::AccountPtr account, const DocumentList documents, QVariantMap requestBase);
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

    Tp::AccountFactoryPtr accountFactory;
    Tp::ConnectionFactoryPtr connectionFactory;
    Tp::ContactFactoryPtr contactFactory;
    Tp::ChannelFactoryPtr channelFactory;
    Tp::AccountManagerPtr accountManager;

private:
    QList<InfTubeServer*> m_serverProcesses;
    void initialize();

private slots:
    void shutdown();
};

#endif
