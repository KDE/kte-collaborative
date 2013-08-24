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

#include "inftube.h"

#include "infinoted.h"
#include "../ui/selecteditorwidget.h"

#include <KTp/debug.h>
#include <KTp/Widgets/contact-grid-dialog.h>
#include <KTp/contact-factory.h>
#include <TelepathyQt/AccountFactory>
#include <TelepathyQt/AccountManager>
#include <TelepathyQt/StreamTubeClient>
#include <TelepathyQt/StreamTubeServer>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/ReferencedHandles>
#include <TelepathyQt/ClientRegistrar>
#include <TelepathyQt/ChannelClassSpecList>
#include <TelepathyQt/ContactManager>
#include <KIO/Job>
#include <KRun>
#include <KDebug>
#include <KMessageBox>
#include <KLocalizedString>
#include <KStandardDirs>
#include <krun.h>

#include <QTcpServer>
#include <QTcpSocket>
#include <unistd.h>

QDBusArgument &operator<<(QDBusArgument &argument, const ChannelList& message) {
    argument.beginArray(qMetaTypeId<QVariantMap>());
    foreach ( const QVariantMap& channel, message ) {
        argument.appendVariant(channel);
    }
    argument.endArray();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ChannelList &message) {
    kDebug() << "unmarshalling";
    argument.beginArray();
    while ( ! argument.atEnd() ) {
        QVariantMap element;
        argument >> element;
        message.append(element);
    }
    argument.endArray();
    kDebug() << "done";
    return argument;
}

QList<Tp::StreamTubeChannelPtr> cleanupChannelList(const QList<Tp::StreamTubeChannelPtr>& list) {
    QList<Tp::StreamTubeChannelPtr> result;
    foreach ( const Tp::StreamTubeChannelPtr& ptr, list ) {
        if ( ptr->ipAddress().second != 0 && ptr->isValid() ) {
            result << ptr;
        }
    }
    return result;
}

InfTubeConnectionMonitor::InfTubeConnectionMonitor(QObject* parent, InfTubeServer* server, InfTubeClient* client)
    : QDBusAbstractAdaptor(parent)
    , server(server)
    , client(client)
{
    Q_ASSERT(!client xor !server);
}

ChannelList InfTubeConnectionMonitor::getChannels()
{
    QList<Tp::StreamTubeChannelPtr> source;
    QString nickname;
    if ( client ) {
        source = client->getChannels();
        nickname = client->nickname();
    }
    else {
        source = server->getChannels();
        nickname = server->nickname();
    }
    ChannelList channels;
    foreach ( const Tp::StreamTubeChannelPtr& channel, source ) {
        QVariantMap result;
        result["channelIdentifier"] = channel->objectPath() + channel->objectName();
        result["targetHandleType"] = channel->targetHandleType();
        result["targetHandle"] = channel->targetContact()->id();
        result["localEndpoint"] = channel->ipAddress().second;
        result["nickname"] = nickname;
        result["accountPath"] = channel->property("accountPath");
        channels << result;
    }
    return channels;
}

ChannelList InfTubeConnectionRetriever::retrieveChannels(QDBusInterface& iface)
{
    QDBusReply<QDBusVariant> reply = iface.call("Get", "org.kde.KTp.infinoteConnectionMonitor", "establishedConnections");
    return qdbus_cast<ChannelList>(reply.value().variant());
}

ChannelList InfTubeConnectionRetriever::retrieveChannels()
{
    qRegisterMetaType< ChannelList >("ChannelList");
    qDBusRegisterMetaType< ChannelList >();
    ChannelList result;
    QDBusInterface serverIface("org.freedesktop.Telepathy.Client.KTp.infinoteServer", "/", "org.freedesktop.DBus.Properties");
    if ( serverIface.isValid() ) {
        result << retrieveChannels(serverIface);
    }
    QDBusInterface clientIface("org.freedesktop.Telepathy.Client.KTp.infinote", "/", "org.freedesktop.DBus.Properties");
    if ( clientIface.isValid() ) {
        result << retrieveChannels(clientIface);
    }
    return result;
}

InfTubeConnectionMonitor::~InfTubeConnectionMonitor()
{

}

const QString serviceName()
{
    return "org.freedesktop.Telepathy.Client.KTp.infinoteServer";
}

InfTubeBase::InfTubeBase(QObject* parent)
    : QObject(parent)
    , m_port(-1)
{

}

InfTubeBase::~InfTubeBase()
{

}

unsigned int InfTubeBase::localPort() const
{
    return m_port;
}

KUrl InfTubeBase::localUrl() const
{
    KUrl url;
    url.setProtocol(QLatin1String("inf"));
    url.setHost(QLatin1String("127.0.0.1"));
    url.setPort(m_port);
    return url;
}

const QString& InfTubeBase::nickname() const
{
    return m_nickname;
}

void InfTubeBase::setNicknameFromAccount(const Tp::AccountPtr& account)
{
    m_nickname = QUrl::toPercentEncoding(
        account->nickname().replace('@', '-')
                           .replace(' ', '_')
    );
}

InfTubeRequester::InfTubeRequester(QObject* parent)
    : InfTubeBase(parent)
{

}

void InfTubeRequester::jobFinished(KJob* job)
{
    KIO::FileCopyJob* j = qobject_cast<KIO::FileCopyJob*>(job);
    Q_ASSERT(j);
    if ( j->error() ) {
        KMessageBox::error(0, i18n("Failed to share file: %1", j->errorString()));
        return;
    }
    KUrl url = j->destUrl();
    url.setUser(nickname());
    emit collaborativeDocumentReady(url);
}

const QVariantMap InfTubeRequester::createHints(const DocumentList& documents) const
{
    QVariantMap hints;
    hints.insert("initialDocumentsSize", documents.size());
    for ( int i = 0; i < documents.size(); i++ ) {
        hints.insert("initialDocument" + QString::number(i), documents.at(i).fileName());
    }
    return hints;
}

bool InfTubeRequester::createRequest(const Tp::AccountPtr account, const DocumentList documents, QVariantMap requestBase)
{
    QVariantMap hints = createHints(documents);
    m_shareDocuments = documents;

    requestBase.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                       TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE);
    requestBase.insert(TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE + QLatin1String(".Service"),
                       QLatin1String("infinote"));

    Tp::PendingChannelRequest* channelRequest;
    channelRequest = account->ensureChannel(requestBase,
                                            QDateTime::currentDateTime(),
                                            "org.freedesktop.Telepathy.Client.KTp.infinoteServer",
                                            hints);

    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)),
            this, SLOT(onTubeRequestReady(Tp::PendingOperation*)));
    return true;
}

void InfTubeRequester::onTubeRequestReady(Tp::PendingOperation* operation)
{
    kDebug() << "TUBE REQUEST FINISHED";
    Tp::ChannelRequestPtr req = qobject_cast<Tp::PendingChannelRequest*>(operation)->channelRequest();
    Tp::StreamTubeChannel* channel = qobject_cast<Tp::StreamTubeChannel*>(req->channel().data());
    kDebug() << "got ST channel" << channel;
    if ( ! channel ) {
        KMessageBox::error(0, i18n("Did not get a valid channel object from Telepathy. Check your installation.<br>"
                                   "Error message was: <b>%1</b>", operation->errorMessage()));
        return;
    }
    QObject::connect(channel->becomeReady(Tp::Features() << Tp::StreamTubeChannel::FeatureCore),
        SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(onTubeReady(Tp::PendingOperation*)));
}

void InfTubeRequester::onTubeReady(Tp::PendingOperation* operation)
{
    kDebug() << "Tube ready:" << operation;
    Tp::PendingReady* ready = qobject_cast<Tp::PendingReady*>(operation);
    Q_ASSERT(ready);
    Tp::StreamTubeChannelPtr channel = Tp::StreamTubeChannelPtr::qObjectCast(ready->proxy());
    Q_ASSERT(channel);
    kDebug() << "parameters:" << channel->parameters();
    if ( ! channel->parameters().contains("localSocket") ) {
        kWarning() << "Got a tube without local socket set -- cannot continue";
        return;
    }
    bool ok = false;
    m_port = channel->parameters()["localSocket"].toUInt(&ok);
    Q_ASSERT(ok);

    // add the initial documents
    foreach ( const KUrl& document, m_shareDocuments ) {
        KUrl x = localUrl();
        x.setFileName(document.fileName());
        KIO::FileCopyJob* job = KIO::file_copy(document, x, -1, KIO::HideProgressInfo);
        connect(job, SIGNAL(finished(KJob*)), this, SLOT(jobFinished(KJob*)));
    }

}

bool InfTubeRequester::offer(const Tp::AccountPtr& account, const Tp::ContactPtr& contact, const DocumentList& documents)
{
    kDebug() << "share with account requested";
    QVariantMap request;
    request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                   (uint) Tp::HandleTypeContact);
    request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetHandle"),
                   contact->handle().at(0));
    return createRequest(account, documents, request);
}

bool InfTubeRequester::offer(const Tp::AccountPtr& account, const QString& chatroom, const DocumentList& documents)
{
    kDebug() << "share with chatroom" << chatroom << "requested";
    QVariantMap request;
    request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                   (uint) Tp::HandleTypeRoom);
    request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetID"),
                   chatroom);
    return createRequest(account, documents, request);
}

bool InfTubeRequester::offer(const Tp::AccountPtr& /*account*/, const Tp::Contacts& /*contact*/, const DocumentList& /*documents*/)
{
    kWarning() << "not implemented";
    return false;
}

QList< Tp::StreamTubeChannelPtr > InfTubeServer::getChannels() const
{
    m_channels = cleanupChannelList(m_channels);
    return m_channels;
}

InfTubeServer::InfTubeServer(QObject* parent)
    : InfTubeBase(parent)
    , m_tubeServer(0)
    , m_hasCreatedChannel(false)
{

}


void InfTubeServer::registerHandler()
{
    kDebug() << "registering handler";
    m_tubeServer = Tp::StreamTubeServer::create(ServerManager::instance()->accountManager, QStringList() << "infinote",
                                                QStringList("infinote"), "KTp.infinoteServer", true);
    m_tubeServer->exportTcpSocket(QHostAddress::LocalHost, 1);
    kDebug() << m_tubeServer->clientName();
    kDebug() << m_tubeServer->isRegistered();
    connect(m_tubeServer.data(), SIGNAL(tubeRequested(Tp::AccountPtr,Tp::OutgoingStreamTubeChannelPtr,QDateTime,Tp::ChannelRequestHints)),
            this, SLOT(tubeRequested(Tp::AccountPtr,Tp::OutgoingStreamTubeChannelPtr,QDateTime,Tp::ChannelRequestHints)));
    connect(m_tubeServer.data(), SIGNAL(tubeClosed(Tp::AccountPtr,Tp::OutgoingStreamTubeChannelPtr,QString,QString)),
            this, SLOT(tubeClosed(Tp::AccountPtr,Tp::OutgoingStreamTubeChannelPtr,QString,QString)));
}

void InfTubeServer::tubeClosed(Tp::AccountPtr , Tp::OutgoingStreamTubeChannelPtr channel, QString , QString )
{
    kDebug() << "tube closed" << channel;
    if ( m_channels.contains(channel) ) {
        m_channels.removeAll(channel);
    }
}

void InfTubeServer::tubeRequested(Tp::AccountPtr account, Tp::OutgoingStreamTubeChannelPtr channel, QDateTime , Tp::ChannelRequestHints requestHints)
{
    kDebug() << "tube requested";
    kDebug() << "is connected:" << channel->state();
    if ( channel->state() == Tp::TubeChannelStateOpen ) {
        // nothing to do
        kDebug() << channel->ipAddress();
        kDebug() << requestHints.allHints();
        return;
    }
    // set infinoted's socket as the local endpoint of the tube
    unsigned short port = -1;
    startInfinoted(&port);

    QVariantMap hints;
    hints = hints.unite(requestHints.allHints());
    hints.insert("localSocket", QString::number(port));

    m_tubeServer->exportTcpSocket(QHostAddress(QHostAddress::LocalHost), port, hints);

    channel->setProperty("accountPath", account->objectPath());
    m_channels.append(channel);
}

QString username() {
#ifdef Q_OS_WIN
    return qgetenv("USERNAME");
#else
    return qgetenv("USER");
#endif
}

QString InfTubeServer::serverDirectory(unsigned short port) const
{
    return QDir::tempPath() + "/infinote-" + username() + "/server-" + QString::number(port);
}

bool InfTubeServer::startInfinoted(unsigned short* port)
{
    // Find a free port by letting the system choose one for a QTcpServer, then closing that
    // server and using the port it was assigned. Arguably not optimal but close enough.
    {
        QTcpServer s;
        s.listen(QHostAddress::LocalHost, 0);
        *port = s.serverPort();
        s.close();
        usleep(200000);
    }
    // Ensure the server directory actually exists
    QDir d(serverDirectory(*port));
    if ( ! d.exists() ) {
        d.mkpath(d.path());
    }
    QProcess* serverProcess = new QProcess;
    m_serverProcesses << serverProcess;
    serverProcess->setEnvironment(QStringList() << "LIBINFINITY_DEBUG_PRINT_TRAFFIC=1");
    serverProcess->setStandardOutputFile(serverDirectory(*port) + "/infinoted.log");
    serverProcess->setStandardErrorFile(serverDirectory(*port) + "/infinoted.errors");
    serverProcess->start(QString(INFINOTED_PATH), QStringList() << "--security-policy=no-tls"
                                           << "-r" << serverDirectory(*port) << "-p" << QString::number(*port));
    serverProcess->waitForStarted(500);
    int timeout = 30; // 30 retries at 100 ms -> 3s
    for ( int i = 0; i < timeout; i ++ ) {
        if ( serverProcess->state() != QProcess::Running ) {
            kWarning() << "server did not start";
            return false;
        }
        QTcpSocket s;
        s.connectToHost("127.0.0.1", *port);
        if ( s.waitForConnected(100) ) {
            break;
        }
    }
    usleep(200000);
    kDebug() << "successfully started infinioted on port" << *port << "( root dir" << serverDirectory(*port) << ")";
    return true;
}

InfTubeServer::~InfTubeServer()
{
    kDebug() << "DESTROYING SERVER";
}

QList<Tp::StreamTubeChannelPtr> InfTubeClient::getChannels() const {
    m_channels = cleanupChannelList(m_channels);
    return m_channels;
};

void InfTubeClient::listen()
{
    kDebug() << "listen called";
    m_tubeClient = Tp::StreamTubeClient::create(ServerManager::instance()->accountManager, QStringList() << "infinote",
                                                QStringList("infinote"), QLatin1String("KTp.infinote"), true, false);
    kDebug() << "tube client: listening";
    m_tubeClient->setToAcceptAsTcp();
    connect(m_tubeClient.data(), SIGNAL(tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr)),
            this, SLOT(tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr)));
    connect(m_tubeClient.data(), SIGNAL(tubeClosed(Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr,QString,QString)),
            this, SLOT(tubeClosed(Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr,QString,QString)));
    kDebug() << m_tubeClient->tubes();
}

void InfTubeClient::tubeClosed(Tp::AccountPtr , Tp::IncomingStreamTubeChannelPtr channel, QString , QString )
{
    kDebug() << "tube closed";
    if ( m_channels.contains(channel) ) {
        m_channels.removeAll(channel);
    }
}

bool InfTubeClient::tryOpenDocument(const KUrl& url)
{
    KUrl dir = url.upUrl();
    KConfig config("ktecollaborative");
    KConfigGroup group = config.group("applications");
    // We do not set a default value here, so the dialog is always
    // displayed the first time the user uses the feature.
    QString command = group.readEntry("editor", "");
    if ( command.isEmpty() ) {
        return false;
    }

    command = command.replace("%u", url.url());
    command = command.replace("%d", dir.url());
    command = command.replace("%h", url.host() % ( url.port() ? (":" + QString::number(url.port())) : QString()));
    QString executable = command.split(' ').first();
    QString arguments = QStringList(command.split(' ').mid(1, -1)).join(" ");
    QString executablePath = KStandardDirs::findExe(executable);
    if ( executablePath.isEmpty() ) {
        return false;
    }
    return KRun::runCommand(executablePath + " " + arguments, 0);
}

void InfTubeClient::tubeAcceptedAsTcp(QHostAddress /*address*/, quint16 port, QHostAddress , quint16 , Tp::AccountPtr account, Tp::IncomingStreamTubeChannelPtr tube)
{
    kDebug() << "Tube accepted as Tcp, port:" << port;
    kDebug() << "parameters:" << tube->parameters();
    // TODO error handling
    m_port = port;
    bool ok = false;
    const int initialSize = tube->parameters().contains("initialDocumentsSize") ? tube->parameters()["initialDocumentsSize"].toInt(&ok) : 0;
    KUrl url = localUrl();
    setNicknameFromAccount(account);
    url.setUser(nickname());
    if ( ! ok || initialSize == 0 ) {
        KRun::runUrl(url.url(), "inode/directory", 0);
    }
    else {
        for ( int i = 0; i < initialSize; i++ ) {
            const QString key = "initialDocument" + QString::number(i);
            const QString path = tube->parameters().contains(key) ? tube->parameters()[key].toString() : QString();
            if ( path.isEmpty() ) {
                kWarning() << "invalid path at index" << i;
                continue;
            }
            url.setPath(path);
            // Retry until the user selects a working application, or aborts
            while ( ! tryOpenDocument(url) ) {
                SelectEditorDialog dlg;
                if ( ! dlg.exec() ) {
                    break;
                }
            }
        }
    }
    tube->setProperty("accountPath", account->objectPath());
    m_channels.append(tube);
    emit connected();
}

InfTubeClient::~InfTubeClient()
{

}

Tp::AccountManagerPtr getAccountManager()
{
    Tp::registerTypes();
    KTp::Debug::installCallback(true);

    Tp::AccountFactoryPtr accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureAvatar
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile
                                                                       << Tp::Account::FeatureCapabilities);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureRosterGroups
                                                                               << Tp::Connection::FeatureRoster
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ContactFactoryPtr contactFactory = KTp::ContactFactory::create(Tp::Features()  << Tp::Contact::FeatureAlias
                                                                      << Tp::Contact::FeatureAvatarData
                                                                      << Tp::Contact::FeatureSimplePresence
                                                                      << Tp::Contact::FeatureCapabilities);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());
    channelFactory->addFeaturesForOutgoingStreamTubes(Tp::Features()
                                                      << Tp::StreamTubeChannel::FeatureConnectionMonitoring
                                                      << Tp::StreamTubeChannel::FeatureCore);
    channelFactory->addFeaturesForIncomingStreamTubes(Tp::Features()
                                                      << Tp::StreamTubeChannel::FeatureConnectionMonitoring
                                                      << Tp::StreamTubeChannel::FeatureCore);

    return Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                      accountFactory,
                                      connectionFactory,
                                      channelFactory,
                                      contactFactory);
}

ServerManager::ServerManager(QObject* parent): QObject(parent)
{
    accountManager = getAccountManager();

    connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(shutdown()));
    connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
}

const ServerManager* InfTubeBase::connectionManager() const
{
    return ServerManager::instance();
}

ServerManager* ServerManager::instance()
{
    static ServerManager* m_self = new ServerManager();
    return m_self;
}

void ServerManager::add(InfTubeServer* server)
{
    m_serverProcesses.append(server);
}

void ServerManager::shutdown()
{
    qDeleteAll(m_serverProcesses);
    m_serverProcesses.clear();
}

#include "inftube.moc"
