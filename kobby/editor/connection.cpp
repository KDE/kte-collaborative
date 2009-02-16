#include "connection.h"
#include "kobbysettings.h"

#include <libqinfinity/ipaddress.h>
#include <libqinfinity/tcpconnection.h>
#include <libqinfinity/xmppconnection.h>

#include <QHostInfo>
#include <QHostAddress>
#include <QDebug>

#include "connection.moc"

namespace Kobby
{

Connection::Connection( const QString &hostname,
    unsigned int port,
    QObject *parent )
    : QObject( parent )
    , m_hostname( hostname )
    , m_port( port )
    , m_tcpConnection( 0 )
    , m_xmppConnection( 0 )
{
}

Connection::~Connection()
{
    m_tcpConnection->close();
    if( m_xmppConnection )
        delete m_xmppConnection;
    if( m_tcpConnection )
        delete m_tcpConnection;
    qDebug() << "Deleting conn.";
}

void Connection::open()
{
    QHostInfo::lookupHost( m_hostname, this,
        SLOT(slotHostnameLookedUp(const QHostInfo&)) );
}

QString Connection::name() const
{
    QString str = m_hostname + ":" + QString::number( m_port );
    return str;
}

QInfinity::XmppConnection *Connection::xmppConnection() const
{
    return m_xmppConnection;
}

void Connection::slotHostnameLookedUp( const QHostInfo &hostInfo )
{
    QList<QHostAddress> addresses = hostInfo.addresses();
    if( addresses.size() == 0 )
    {
        emit(error( this, "Host not found." ));
        return;
    }

    m_tcpConnection = new QInfinity::TcpConnection( QInfinity::IpAddress( addresses[0] ),
        m_port,
        this );

    m_xmppConnection = new QInfinity::XmppConnection( *m_tcpConnection,
        QInfinity::XmppConnection::Client,
        KobbySettings::hostName(),
        m_hostname,
        QInfinity::XmppConnection::PreferTls,
        0, 0, 0,
        this );

    connect( m_xmppConnection, SIGNAL(statusChanged()),
        this, SLOT(slotStatusChanged()) );
    connect( m_xmppConnection, SIGNAL(error( const GError* )),
        this, SLOT(slotError( const GError* )) );

    m_tcpConnection->open();
}

void Connection::slotStatusChanged()
{
    switch( m_xmppConnection->status() )
    {
        case QInfinity::XmlConnection::Opening:
            emit(connecting( this ));
            break;
        case QInfinity::XmlConnection::Closing:
            emit(disconnecting( this ));
            break;
        case QInfinity::XmlConnection::Open:
            emit(connected( this ));
            break;
        case QInfinity::XmlConnection::Closed:
            emit(disconnected( this ));
    }
}

void Connection::slotError( const GError *err )
{
    emit(error( this, QString(err->message) ));
}

}

