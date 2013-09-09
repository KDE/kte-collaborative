/*
 * Copyright 2013 Sven Brauch <svenbrauch@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef STATUSOVERLAY_H
#define STATUSOVERLAY_H

#include <QDeclarativeView>
#include <libqinfinity/xmlconnection.h>

#include "common/document.h"

struct Connection;
using Kobby::Document;
namespace KTextEditor {
    class View;
}

class StatusOverlay : public QDeclarativeView
{
Q_OBJECT
public:
    explicit StatusOverlay(KTextEditor::View* parent);
    virtual bool eventFilter(QObject* watched, QEvent* e);
    void displayText(const QString& text);

public slots:
    void progress(double percentage);
    void loadStateChanged(Document*,Document::LoadState);
    void connectionStatusChanged(Connection*,QInfinity::XmlConnection::Status);

private:
    void resizeToView();
    void setProgressBar(double percentage);

private:
    KTextEditor::View* m_view;
    QTime m_maxUpdateRateTimer;
};

#endif // STATUSOVERLAY_H
