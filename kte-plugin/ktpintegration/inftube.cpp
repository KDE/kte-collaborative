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
#include <unistd.h>
#include <KIO/Job>
#include <KRun>
#include <KDebug>
#include <KMessageBox>
#include <KLocalizedString>
#include <krun.h>

#include <QTcpServer>
#include <QTcpSocket>

const QString serviceName()
{
    return "org.freedesktop.Telepathy.Client.KTp.infinote-server";
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

InfTubeServer::InfTubeServer(QObject* parent)
    : InfTubeBase(parent)
    , m_tubeServer(0)
    , m_hasCreatedChannel(false)
{
//     ServerManager::instance()->add(this);
}

// void InfTubeServer::jobFinished(KJob* job)
// {
//     KIO::FileCopyJob* j = qobject_cast<KIO::FileCopyJob*>(job);
//     Q_ASSERT(j);
//     if ( j->error() ) {
//         KMessageBox::error(0, i18n("Failed to share file: %1", j->errorString()));
//         return;
//     }
//     KUrl url = j->destUrl();
//     url.setUser(nickname());
//     emit fileCopiedToServer(url);
// }

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

    // add the initial documents
    foreach ( const KUrl& document, documents ) {
        KUrl x = localUrl();
        x.setFileName(document.fileName());
        KIO::FileCopyJob* job = KIO::file_copy(document, x, -1, KIO::HideProgressInfo);
        connect(job, SIGNAL(finished(KJob*)), this, SLOT(jobFinished(KJob*)));
    }

    requestBase.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                       TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE);
    requestBase.insert(TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE + QLatin1String(".Service"),
                       QLatin1String("infinote"));

    Tp::PendingChannelRequest* channelRequest;
    channelRequest = account->ensureChannel(requestBase,
                                            QDateTime::currentDateTime(),
                                            "org.freedesktop.Telepathy.Client." + serviceName(),
                                            hints);

    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)),
            this, SLOT(onTubeRequestReady(Tp::PendingOperation*)));
    return true;
}

void InfTubeRequester::onTubeRequestReady(Tp::PendingOperation* )
{
    kDebug() << "TUBE REQUEST FINISHED";
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

void InfTubeServer::registerHandler()
{
    kDebug() << "registering handler with service name" << serviceName();
    m_tubeServer = Tp::StreamTubeServer::create(ServerManager::instance()->accountManager, QStringList() << "infinote-server",
                                                QStringList(), serviceName());
    connect(m_tubeServer.data(), SIGNAL(tubeRequested(Tp::AccountPtr,Tp::OutgoingStreamTubeChannelPtr,QDateTime,Tp::ChannelRequestHints)),
            this, SLOT(tubeRequested(Tp::AccountPtr,Tp::OutgoingStreamTubeChannelPtr,QDateTime,Tp::ChannelRequestHints)));
}

void InfTubeServer::tubeRequested(Tp::AccountPtr , Tp::OutgoingStreamTubeChannelPtr channel, QDateTime , Tp::ChannelRequestHints )
{
    kDebug() << "tube requested";
    kDebug() << channel->ipAddress();
    // set infinoted's socket as the local endpoint of the tube
    unsigned short port = -1;
    startInfinoted(&port);
    QVariantMap hints;
    hints.insert("localSocket", QString::number(port));
    m_tubeServer->exportTcpSocket(QHostAddress(QHostAddress::LocalHost), port, hints);
}

QString InfTubeServer::serverDirectory(unsigned short port) const
{
    return QDir::tempPath() + "infinote/" + QString::number(port);
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
        s.connectToHost("localhost", *port);
        if ( s.waitForConnected(100) ) {
            break;
        }
    }
    kDebug() << "successfully started infinioted on port" << *port << "( root dir" << serverDirectory(*port) << ")";
    return true;
}

InfTubeServer::~InfTubeServer()
{
    kDebug() << "DESTROYING SERVER";
}

void InfTubeClient::listen()
{
    kDebug() << "listen called";
    m_tubeClient = Tp::StreamTubeClient::create(ServerManager::instance()->accountManager, QStringList() << "infinote",
                                                QStringList(), QLatin1String("KTp.infinote"), true, true);
    kDebug() << "tube client: listening";
    m_tubeClient->setToAcceptAsTcp();
    connect(m_tubeClient.data(), SIGNAL(tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr)),
            this, SLOT(tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr)));
    kDebug() << m_tubeClient->tubes();
}

void InfTubeClient::tubeAcceptedAsTcp(QHostAddress /*address*/, quint16 port, QHostAddress , quint16 , Tp::AccountPtr account, Tp::IncomingStreamTubeChannelPtr tube)
{
    kDebug() << "Tube accepted as Tcp, port:" << port;
    kDebug() << "parameters:" << tube->parameters();
    // TODO error handling
    // TODO proper selection of application(s) to run
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
            KConfig config("ktecollaborative");
            KConfigGroup group = config.group("applications");
            KRun::run(group.readEntry("editor", "kwrite %u"), KUrl::List() << url, 0);
        }
    }
    emit connected();
}

InfTubeClient::~InfTubeClient()
{

}

ServerManager::ServerManager(QObject* parent): QObject(parent)
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

    accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory,
                                                  contactFactory);

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
