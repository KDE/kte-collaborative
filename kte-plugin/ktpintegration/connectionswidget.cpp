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

int ConnectionsModel::rowCount(const QModelIndex& parent) const
{
    kDebug() << "row count called";
    return m_connections.count();
}

int ConnectionsModel::columnCount(const QModelIndex& parent) const
{
    kDebug() << "column count called";
    return 4;
}

QVariant ConnectionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( role == Qt::DisplayRole && orientation == Qt::Horizontal ) {
        if ( section == 3 ) {
            return "Channel identifier";
        }
        else if ( section == 1 ) {
            return "Target handle type";
        }
        else if ( section == 2 ) {
            return "Target handle";
        }
        else if ( section == 0 ) {
            return "Local endpoint";
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant ConnectionsModel::data(const QModelIndex& index, int role) const
{
    kDebug() << "data called" << index << role;
    if ( role == Qt::DisplayRole ) {
        switch ( index.column() ) {
            case 3:
                return m_connections.at(index.row())["channelIdentifier"];
            case 1:
                return m_connections.at(index.row())["targetHandleType"];
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
}

#include "connectionswidget.moc"
