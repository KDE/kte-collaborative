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

#ifndef CONNECTIONSWIDGET_H
#define CONNECTIONSWIDGET_H

#include "inftube_export.h"
#include "inftube.h"

#include <QWidget>
#include <QAbstractListModel>

class QTableView;

// This is currently more of a debug-oriented than a user-oriented view.
// It needs to be adjusted later on.
class ConnectionsModel : public QAbstractTableModel {
public:
    explicit ConnectionsModel(QObject* parent = 0);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    ChannelList m_connections;
};

class INFTUBE_EXPORT ConnectionsWidget : public QWidget {
Q_OBJECT
public:
    ConnectionsWidget();

private slots:
    void adjustTableSizes();

private:
    QTableView* m_connectionsView;
};

#endif // CONNECTIONSWIDGET_H
