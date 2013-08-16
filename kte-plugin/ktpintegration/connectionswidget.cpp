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

#include "inftube.h"

ConnectionsModel::ConnectionsModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    InfTubeConnectionRetriever r;
    m_connections = r.retrieveChannels();
    kDebug() << "channels:" << m_connections;
    emit dataChanged(index(0, 0), index(rowCount(), columnCount(QModelIndex())));
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
//         if ( section == 3 ) {
//             return "Channel identifier";
//         }
        if ( section == 1 ) {
            return "Type";
        }
        else if ( section == 2 ) {
            return "Peer";
        }
        else if ( section == 0 ) {
            return "Port";
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant ConnectionsModel::data(const QModelIndex& index, int role) const
{
    if ( role == Qt::DisplayRole ) {
        switch ( index.column() ) {
//             case 3:
//                 return m_connections.at(index.row())["channelIdentifier"];
            case 1: {
                int type = m_connections.at(index.row())["targetHandleType"].toInt();
                if ( type == Tp::HandleTypeContact ) {
                    return "Contact";
                }
                else if ( type == Tp::HandleTypeRoom ) {
                    return "Chatroom";
                }
                else {
                    return "unknown";
                }
            }
            case 2:
                return m_connections.at(index.row())["targetHandle"];
            case 0:
                return m_connections.at(index.row())["localEndpoint"];
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

ConnectionsWidget::ConnectionsWidget()
{
    kDebug() << "creating connections widget";
    m_connectionsView = new QTableView();
    setLayout(new QHBoxLayout());
    layout()->addWidget(m_connectionsView);
    ConnectionsModel* model = new ConnectionsModel(m_connectionsView);
    m_connectionsView->setModel(model);
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(adjustTableSizes()));
    adjustTableSizes();
    m_connectionsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(m_connectionsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(rowClicked(QModelIndex)));
}

#include "connectionswidget.moc"
