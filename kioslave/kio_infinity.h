/*  This file is part of kobby
    Copyright (c) 2013 Sven Brauch <svenbrauch@gmail.com>


    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef __kio_infinity_h__
#define __kio_infinity_h__

#include <kio/global.h>
#include <kio/slavebase.h>

#include <common/connection.h>
#include <common/noteplugin.h>
#include <libqinfinity/browsermodel.h>

#include <libinfinity/client/infc-request.h>

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

class InfinityProtocol : public QObject, public KIO::SlaveBase
{
    Q_OBJECT

public:

    InfinityProtocol(const QByteArray &pool_socket, const QByteArray &app_socket);
    virtual ~InfinityProtocol();

    virtual void get(const KUrl& url);
    virtual void stat(const KUrl& url);

    virtual void mimetype(const KUrl& url);
    virtual void listDir(const KUrl& url);
    virtual void put(const KUrl& url, int permissions, KIO::JobFlags flags);
    virtual void mkdir(const KUrl& url, int permissions);


    static InfinityProtocol* self();

    // Callback function for the infinity request
    static void requestError_cb(InfcRequest* request, GError* error, void* user_data);

signals:
    // This signal is emitted if a request fails. Since it is connected to a local
    // event loop's quit() slot usually, the error message is available through
    // the m_lastError member. You have to reset that member if you handeled the error.
    void requestError();

    // This signal should be emitted if an operation was successful, and the
    // waitForRequest() function should exit.
    void requestSuccessful();

private:
    // wrapper for emitting error signal if a request fails
    void signalError(const QString);

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
    QInfinity::BrowserIter iterForUrl(const KUrl& url);

    // Get the browser for the currently established connection.
    // Only call this if connected.
    QInfinity::Browser* browser() const;

    // Waits for the given InfcRequest to finish, and reacts to errors accordingly.
    // A slave function (such as put()) should just abort (return) if this returns false.
    bool waitForRequest(const InfcRequest* request);

    static InfinityProtocol* _self;
    QSharedPointer<Kobby::Connection> m_connection;
    QSharedPointer<QInfinity::BrowserModel> m_browserModel;
    Kobby::NotePlugin* m_notePlugin;
    Peer m_connectedTo;
    QString m_lastError;

};


#endif
