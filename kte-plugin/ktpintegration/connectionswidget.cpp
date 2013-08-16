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
#include "inftube.h"
#include <QListView>
#include <QHBoxLayout>
#include <QLayout>

ConnectionsModel::ConnectionsModel(QObject* parent)
    : QAbstractListModel(parent)
{
    InfTubeConnectionRetriever r;
    m_connections = r.retrieveChannels();
    kDebug() << "channels:" << m_connections;
    emit dataChanged(index(0), index(rowCount()));
}

int ConnectionsModel::rowCount(const QModelIndex& parent) const
{
    kDebug() << "row count called";
    return m_connections.count();
}

int ConnectionsModel::columnCount(const QModelIndex& parent) const
{
    kDebug() << "column count called";
    return 3;
}

QVariant ConnectionsModel::data(const QModelIndex& index, int role) const
{
    kDebug() << "data called" << index << role;
    if ( role == Qt::DisplayRole ) {
        return m_connections.at(index.row())["channelIdentifier"];
    }
    return QVariant();
}

ConnectionsWidget::ConnectionsWidget()
{
    kDebug() << "creating connections widget";
    m_connectionsView = new QListView();
    setLayout(new QHBoxLayout());
    layout()->addWidget(m_connectionsView);
    ConnectionsModel* model = new ConnectionsModel(m_connectionsView);
    m_connectionsView->setModel(model);
}
