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

#ifndef __kio_infinity_h__
#define __kio_infinity_h__

#include "common/connection.h"
#include "common/noteplugin.h"

#include <kio/global.h>
#include <kio/slavebase.h>

#include <libqinfinity/browsermodel.h>
#include <libqinfinity/browseriter.h>

#include <libinfinity/client/infc-request.h>

namespace QInfinity {
    class NodeRequest;
}

using QInfinity::NodeRequest;
using QInfinity::BrowserIter;

/**
 * @brief Represents a host/port pair.
 */
struct Peer {
    Peer(QString hostname, int port = -1)
        : hostname(hostname)
        , port(port == -1 ? 6523 : port) { };
    Peer(const KUrl& url)
        : hostname(url.host())
        , port(url.port() == -1 ? 6523 : url.port()) { };
    Peer()
        : hostname(QString())
        , port(-1) { };
    bool operator==(const Peer& other) const {
        return hostname == other.hostname && port == other.port;
    }
    bool operator!=(const Peer& other) const {
        return ! operator==(other);
    }
    QString hostname;
    int port;
};

/**
 * @brief Main class for the KIO slave
 */
class InfinityProtocol : public QObject, public KIO::SlaveBase
{
Q_OBJECT

public:
    InfinityProtocol(const QByteArray &pool_socket, const QByteArray &app_socket);
    virtual ~InfinityProtocol() { };

    virtual void get(const KUrl& url);
    virtual void stat(const KUrl& url);

    virtual void mimetype(const KUrl& url);
    virtual void listDir(const KUrl& url);
    virtual void put(const KUrl& url, int permissions, KIO::JobFlags flags);
    virtual void mkdir(const KUrl& url, int permissions);
    virtual void del(const KUrl& url, bool isfile);

signals:
    // This signal is emitted if a request fails.
    // message provides a human-readable description of why the request failed.
    // TODO translate messages? How? I guess they need to be translated
    // in infinoted.
    void requestError(GError* error);

    // This signal is be emitted if an operation was successful.
    void requestSuccessful(NodeRequest* req);

public slots:
    void slotRequestError(GError* error);

private:
    // Checks if a connection to the given peer is open already.
    bool isConnectedTo(const Peer& peer);

    // Establish a connection to the given peer.
    // Sets an appropriate error status if connecting fails, and returns false
    // in that case.
    // If a connection is already open for the given peer, does nothing
    // and returns true.
    bool doConnect(const Peer& peer);

    // Finds a QInfinity::BrowserIter for the given URL. This operation requires
    // communication with the server and is very expensive.
    // You can provide an "ok" boolean to check if the request succeeded,
    // in case you are not sure it does. If ok is false, the returned
    // iter is invalid.
    QInfinity::BrowserIter iterForUrl(const KUrl& url, bool* ok = 0);

    // Get the browser for the currently established connection.
    // Only call this if connected.
    QInfinity::Browser* browser() const;

    // Waits for a request finish (as signaled by requestSuccessful() / requestError()),
    // and reacts to errors accordingly.
    // A slave function (such as put()) should just abort (return) if this returns false.
    bool waitForCompletion();

    QSharedPointer<Kobby::Connection> m_connection;
    QSharedPointer<QInfinity::BrowserModel> m_browserModel;
    Kobby::NotePlugin* m_notePlugin;
    Peer m_connectedTo;
    QString m_lastError;
};


#endif
