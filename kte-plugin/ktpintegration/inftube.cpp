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

#include <KTp/debug.h>
#include <KTp/Widgets/contact-grid-dialog.h>
#include <KTp/contact-factory.h>
#include <telepathy-qt4/TelepathyQt/AccountFactory>
#include <telepathy-qt4/TelepathyQt/AccountManager>
#include <telepathy-qt4/TelepathyQt/StreamTubeClient>
#include <telepathy-qt4/TelepathyQt/StreamTubeServer>
#include <telepathy-qt4/TelepathyQt/PendingChannelRequest>
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

ConnectionManager::ConnectionManager(QObject* parent): QObject(parent)
{
    Tp::registerTypes();
    KTp::Debug::installCallback(true);

    accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureAvatar
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile);

    connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureRosterGroups
                                                                               << Tp::Connection::FeatureRoster
                                                                               << Tp::Connection::FeatureSelfContact);

    contactFactory = KTp::ContactFactory::create(Tp::Features()  << Tp::Contact::FeatureAlias
                                                                      << Tp::Contact::FeatureAvatarData
                                                                      << Tp::Contact::FeatureSimplePresence
                                                                      << Tp::Contact::FeatureCapabilities);

    channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory,
                                                  contactFactory);

    connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(shutdown()));
    connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
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

InfTubeBase::ConnectionStatus InfTubeBase::status() const
{
    return m_status;
}

const QString InfTubeServer::serviceName() const
{
    return "KTp.infserver" + QString::number(QApplication::instance()->applicationPid());
}

InfTubeServer::InfTubeServer(QObject* parent)
{
    ConnectionManager::instance()->add(this);
    m_tubeServer = Tp::StreamTubeServer::create(ConnectionManager::instance()->accountManager, QStringList() << "infinity",
                                                QStringList(), serviceName());
}

bool InfTubeServer::offer(Tp::AccountPtr account, const Tp::ContactPtr contact, const KUrl& document)
{
    return offer(account, ContactList() << contact, DocumentList() << document);
}

bool InfTubeServer::offer(Tp::AccountPtr account, const ContactList& contacts, const DocumentList& documents)
{
    kDebug() << "starting infinoted";
    if ( ! startInfinoted() ) {
        // TODO user-visible error handling
        kWarning() << "Failed to start infinoted. Check settings and installation.";
        return false;
    }
    // construct a map of documents to open initially
    QVariantMap hints;
    hints.insert("initialDocumentsSize", documents.size());
    for ( int i = 0; i < documents.size(); i++ ) {
        hints.insert("initialDocument" + QString::number(i), documents.at(i).fileName());
    }
    // set infinoted's socket as the local endpoint of the tube
    m_tubeServer->exportTcpSocket(QHostAddress(QHostAddress::LocalHost), m_port, hints);
    // add the initial documents
    foreach ( const KUrl& document, documents ) {
        KUrl x = localUrl();
        x.setFileName(document.fileName());
        KIO::TransferJob* job = KIO::put(x, -1);
    }
    QVariantMap request;
    request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                   TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE);
    request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                   (uint) Tp::HandleTypeContact);
    request.insert(TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE + QLatin1String(".Service"),
                   QLatin1String("infinity"));
    // TODO !!!
    request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetHandle"),
                   contacts.first()->handle().at(0));
    Tp::PendingChannelRequest* channelRequest;
    channelRequest = account->ensureChannel(request,
                                            QDateTime::currentDateTime(),
                                            "org.freedesktop.Telepathy.Client." + serviceName());

    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)),
            this, SLOT(onCreateTubeFinished(Tp::PendingOperation*)));
    return true;
}

void InfTubeServer::onCreateTubeFinished(Tp::PendingOperation* operation)
{
    kDebug() << "create tube finished; is error:" << operation->isError();
    kDebug() << "error message:" << operation->errorMessage();
}

bool InfTubeServer::startInfinoted()
{
    // Find a free port by letting the system choose one for a QTcpServer, then closing that
    // server and using the port it was assigned. Arguably not optimal but close enough.
    {
        QTcpServer s;
        s.listen(QHostAddress::LocalHost, 0);
        m_port = s.serverPort();
        s.close();
    }
    // Ensure the server directory actually exists
    QDir d(serverDirectory());
    if ( ! d.exists() ) {
        d.mkpath(d.path());
    }
    m_serverProcess = new QProcess;
    m_serverProcess->setEnvironment(QStringList() << "LIBINFINITY_DEBUG_PRINT_TRAFFIC=1");
    m_serverProcess->setStandardOutputFile(serverDirectory() + "/infinoted.log");
    m_serverProcess->setStandardErrorFile(serverDirectory() + "/infinoted.errors");
    // TODO windows?
    m_serverProcess->start("/usr/bin/env", QStringList() << "infinoted-0.5" << "--security-policy=no-tls"
                                           << "-r" << serverDirectory() << "-p" << QString::number(m_port));
    m_serverProcess->waitForStarted(500);
    int timeout = 30; // 30 retries at 100 ms -> 3s
    for ( int i = 0; i < timeout; i ++ ) {
        if ( m_serverProcess->state() != QProcess::Running ) {
            kWarning() << "server did not start";
            return false;
        }
        QTcpSocket s;
        s.connectToHost("localhost", m_port);
        if ( s.waitForConnected(100) ) {
            break;
        }
    }
    kDebug() << "successfully started infinioted on port" << m_port << "( root dir" << serverDirectory() << ")";
    return true;
}

const QString InfTubeServer::serverDirectory() const
{
    return QDir::tempPath() + "/infinoted-" + QString::number(QApplication::instance()->applicationPid());
}

InfTubeServer::~InfTubeServer()
{
    m_serverProcess->terminate();
}

void InfTubeClient::listen()
{
    kDebug() << "listen called";
    m_tubeClient = Tp::StreamTubeClient::create(ConnectionManager::instance()->accountManager, QStringList() << "infinity",
                                                QStringList(), QLatin1String("KTp.infinity"), true, true);
    kDebug() << "tube client: listening";
    m_tubeClient->setToAcceptAsTcp();
    connect(m_tubeClient.data(), SIGNAL(tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr)),
            this, SLOT(tubeAcceptedAsTcp(QHostAddress,quint16,QHostAddress,quint16,Tp::AccountPtr,Tp::IncomingStreamTubeChannelPtr)));
    kDebug() << m_tubeClient->tubes();
}

void InfTubeClient::tubeAcceptedAsTcp(QHostAddress address, quint16 port, QHostAddress , quint16 , Tp::AccountPtr account, Tp::IncomingStreamTubeChannelPtr tube)
{
    kDebug() << "Tube accepted as Tcp, port:" << port;
    kDebug() << "parameters:" << tube->parameters();
    // TODO error handling
    // TODO proper selection of application(s) to run
    m_port = port;
    bool ok = false;
    const int initialSize = tube->parameters().contains("initialDocumentsSize") ? tube->parameters()["initialDocumentsSize"].toInt(&ok) : 0;
    KUrl url = localUrl();
    url.setUser(compatibleNickname(account));
    if ( ! ok || initialSize == 0 ) {
        KRun::run("dolphin " + url.url(), KUrl::List(), 0);
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
            KRun::run("kwrite " + url.url(), KUrl::List(), 0);
        }
    }
    emit connected();
}

InfTubeClient::~InfTubeClient()
{

}

QString InfTubeBase::compatibleNickname(const Tp::AccountPtr& account)
{
    return QUrl::toPercentEncoding(account->nickname().replace('@', '-'));
}

ConnectionManager* ConnectionManager::instance()
{
    static ConnectionManager* m_self = new ConnectionManager();
    return m_self;
}

void ConnectionManager::add(InfTubeServer* server)
{
    m_serverProcesses.append(server);
}

void ConnectionManager::shutdown()
{
    qDeleteAll(m_serverProcesses);
    m_serverProcesses.clear();
}

#include "inftube.moc"
