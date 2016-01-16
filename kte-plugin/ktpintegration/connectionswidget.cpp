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

#include "connectionswidget.h"

#include <QHBoxLayout>
#include <QLayout>
#include <QTableView>
#include <QHeaderView>
#include <QLabel>
#include <QStackedWidget>
#include <KLocalizedString>

#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingContacts>
#include <TelepathyQt/PendingReady>

#include "inftube.h"

ConnectionsModel::ConnectionsModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    m_accountManager = getAccountManager();
    connect(m_accountManager->becomeReady(Tp::Features() << Tp::AccountManager::FeatureCore),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}

void ConnectionsModel::onAccountManagerReady(Tp::PendingOperation* )
{
    InfTubeConnectionRetriever r;
    const ChannelList channels = r.retrieveChannels();
    if ( channels.size() > 0 ){
        beginInsertRows(QModelIndex(), 0, channels.size() - 1);
        m_connections = channels;
        endInsertRows();
    }
    qDebug() << "channels:" << m_connections;
    foreach ( const QVariantMap& channelData, m_connections ) {
        qDebug() << "constructing tube for channel" << channelData;
        qDebug() << "accounts:" << m_accountManager->allAccounts();
        foreach ( const Tp::AccountPtr account,  m_accountManager->allAccounts() ) {
            qDebug() << account->objectPath();
        }
        Tp::AccountPtr account = m_accountManager->accountForPath(channelData["accountPath"].toString());
        Tp::StreamTubeChannelPtr channel = Tp::StreamTubeChannel::create(account->connection(),
                                                                         channelData["channelIdentifier"].toString(),
                                                                         QVariantMap());
        m_channels << channel;
        connect(channel->becomeReady(Tp::Features() << Tp::StreamTubeChannel::FeatureCore),
                SIGNAL(finished(Tp::PendingOperation*)),
                SLOT(onChannelReady(Tp::PendingOperation*)));
    }
}

void ConnectionsModel::onChannelReady(Tp::PendingOperation* operation)
{
    qDebug() << "channel ready" << rowCount() << "channels total";
    Tp::StreamTubeChannelPtr channel;
    channel = Tp::StreamTubeChannelPtr::qObjectCast(qobject_cast<Tp::PendingReady*>(operation)->proxy());
    int i = -1;
    for ( ChannelList::iterator it = m_connections.begin(); it != m_connections.end(); it++ ) {
        i += 1;
        qDebug() << "correct id:" << ((*it)["channelIdentifier"].toString() == channel->objectPath());
        if ( (*it)["channelIdentifier"].toString() != channel->objectPath() ) {
            continue;
        }
        (*it)["icon"] = channel->targetContact()->avatarData().fileName;
        dataChanged(index(i, 0), index(i, columnCount(QModelIndex())));
    }
}

int ConnectionsModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_connections.count();
}

int ConnectionsModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 3;
}

QVariant ConnectionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( role == Qt::DisplayRole && orientation == Qt::Horizontal ) {
        if ( section == 1 ) {
            return i18n("Type");
        }
        else if ( section == 2 ) {
            return i18n("Peer");
        }
        else if ( section == 0 ) {
            return i18n("Port");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant ConnectionsModel::data(const QModelIndex& index, int role) const
{
    if ( role == Qt::DecorationRole ) {
        switch ( index.column() ) {
            case 2: {
                const QVariantMap& channel = m_connections.at(index.row());
                int type = channel["targetHandleType"].toInt();
                qDebug() << channel;
                if ( type == Tp::HandleTypeContact ) {
                    return QIcon(channel["icon"].toString());
                }
            }
            default:
                return QVariant();
        }
    }
    if ( role == Qt::DisplayRole ) {
        switch ( index.column() ) {
            case 0:
                return m_connections.at(index.row())["localEndpoint"];
            case 1: {
                int type = m_connections.at(index.row())["targetHandleType"].toInt();
                if ( type == Tp::HandleTypeContact ) {
                    return i18n("Contact");
                }
                else if ( type == Tp::HandleTypeRoom ) {
                    return i18n("Chatroom");
                }
                else {
                    return i18n("unknown");
                }
            }
            case 2:
                return m_connections.at(index.row())["targetHandle"];
        }
    }
    return QVariant();
}

void ConnectionsWidget::adjustTableSizes()
{
    m_connectionsView->resizeColumnsToContents();
    m_connectionsView->resizeRowsToContents();
    m_connectionsView->horizontalHeader()->setStretchLastSection(true);
}

void ConnectionsWidget::rowClicked(QModelIndex index)
{
    ConnectionsModel* model = static_cast<ConnectionsModel*>(m_connectionsView->model());
    const QVariantMap& channel = model->m_connections.at(index.row());
    emit connectionClicked(channel["localEndpoint"].toInt(), channel["nickname"].toString());
}

void ConnectionsWidget::setHelpMessage(const QString& message)
{
    m_helpMessageLabel->setText(message);
    m_helpMessageLabel->setVisible(true);
}

ConnectionsWidget::ConnectionsWidget(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    qDebug() << "creating connections widget";

    // Build the widget for table + help message, if applicable
    QWidget* tableWidget = new QWidget();
    tableWidget->setLayout(new QVBoxLayout());
    m_connectionsView = new QTableView();
    ConnectionsModel* model = new ConnectionsModel(m_connectionsView);
    m_connectionsView->setModel(model);
    m_helpMessageLabel = new QLabel();
    m_helpMessageLabel->setVisible(false); // will be set to visible by setHelpMessage()

    connect(model, SIGNAL(rowsInserted(const QModelIndex&,int,int)),
            this, SLOT(adjustTableSizes()));
    connect(model, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
            this, SLOT(adjustTableSizes()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(checkIfEmpty()));
    m_connectionsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(m_connectionsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(rowClicked(QModelIndex)));

    tableWidget->layout()->addWidget(m_helpMessageLabel);
    tableWidget->layout()->addWidget(m_connectionsView);

    // Build the label if thre's no connections
    m_noConnectionsLabel = new QLabel("<i>" % i18n("No active connections.") % "</i>");
    m_noConnectionsLabel->setAlignment(Qt::AlignHCenter);

    setLayout(new QHBoxLayout);
    m_stack = new QStackedWidget;
    m_stack->addWidget(m_noConnectionsLabel);
    m_stack->addWidget(tableWidget);
    layout()->addWidget(m_stack);

    checkIfEmpty();
    adjustTableSizes();
}

void ConnectionsWidget::checkIfEmpty()
{
    if ( m_connectionsView->model()->rowCount() == 0 ) {
        m_stack->setCurrentIndex(0);
    }
    else {
        m_stack->setCurrentIndex(1);
    }
}

#include "connectionswidget.moc"
