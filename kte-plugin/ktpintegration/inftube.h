/***************************************************************************
 *   Copyright (C) %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>                 *
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

#include <KJob>
#include <KUrl>

namespace Tp {
    class StreamTubeClient;
}

typedef QList<const Tp::ContactPtr> ContactList;
typedef QList<const KUrl> DocumentList;

/**
 * This class defines the API for routing infinity traffic through a TP tube
 * which is common to both the receiving an the offering side.
 */
class InfTubeBase : public QObject {
Q_OBJECT
public:
    InfTubeBase(QObject* parent = 0) = 0;
    virtual ~InfTubeBase() = 0;

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

signals:
    /**
     * @brief Emitted when the connection has been established
     */
    void connected(InfTubeBase* self);


protected:
    Tp::AccountManagerPtr m_accountManager;
    ConnectionStatus m_status;
    unsigned int m_port;

    /**
     * @brief Performs some initialization tasks. Call before everything else.
     */
    void initialize();
};

/**
 * @brief This class implements InfTubeBase and is used on the receiving end.
 */
class InfTubeClient : public InfTubeBase {
public:
    InfTubeClient(QObject* parent = 0);
    virtual ~InfTubeClient();

    /**
     * @brief Begin waiting for incoming connections.
     */
    void listen();

private:
    Tp::StreamTubeClientPtr m_tubeClient;
public slots:
    void tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr);
};

/**
 * @brief This class implements InfTubeBase and can be used to offer some documents to a contact.
 */
class InfTubeServer : public InfTubeBase {
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
    bool offer(const Tp::Account& account, const ContactList& contacts, const DocumentList& initialDocuments);

    /**
     * @brief Convenience overload, for offering a single document to a single contact.
     */
    bool offer(const Tp::Account& account, const Tp::ContactPtr& contact, const KUrl& document);

private:
    Tp::StreamTubeServerPtr m_tubeServer;
    QProcess* m_serverProcess;

    void startInfinoted();
    const QString serverDirectory() const;
};

#endif
