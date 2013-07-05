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

typedef QList<Tp::ContactPtr> ContactList;
typedef QList<KUrl> DocumentList;

/**
 * This class defines the API for routing infinity traffic through a TP tube
 * which is common to both the receiving an the offering side.
 */
class INFTUBE_EXPORT InfTubeBase {
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
     * @brief Get the status of the connection.
     */
    ConnectionStatus status() const;

//     /**
//      * @brief Get the client registrar instance.
//      */
//     Tp::ClientRegistrarPtr clientRegistrar() const;

protected:
    Tp::AccountManagerPtr m_accountManager;
    ConnectionStatus m_status;
    unsigned int m_port;
//     Tp::ClientRegistrarPtr m_registrar;

    /**
     * @brief Performs some initialization tasks. Call before everything else.
     */
    void initialize();
};


inline Tp::ChannelClassSpecList channelClassList()
{
    return Tp::ChannelClassSpecList() << Tp::ChannelClassSpec::incomingStreamTube("infinity");
}

/**
 * @brief This class implements InfTubeBase and is used on the receiving end.
 */
class INFTUBE_EXPORT InfTubeClient : public QObject, public InfTubeBase {
Q_OBJECT
public:
    explicit InfTubeClient() {
        // TODO move to cpp
        initialize();
    };
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
    void tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr);
};

/**
 * @brief This class implements InfTubeBase and can be used to offer some documents to a contact.
 */
class INFTUBE_EXPORT InfTubeServer : public QObject, public InfTubeBase {
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
     * @brief Convenience overload, for offering a single document to a single contact.
     */
    bool offer(Tp::AccountPtr account, const Tp::ContactPtr contact, const KUrl& document);

signals:
    /**
     * @brief Emitted when the connection has been established
     */
    void connected(InfTubeBase* self);

private slots:
    void onCreateTubeFinished(Tp::PendingOperation*);

private:
    Tp::StreamTubeServerPtr m_tubeServer;
    QProcess* m_serverProcess;

    bool startInfinoted();
    const QString serverDirectory() const;
    const QString serviceName() const;
};

#endif
