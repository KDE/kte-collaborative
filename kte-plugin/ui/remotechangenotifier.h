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

#ifndef REMOTECHANGENOTIFIER_H
#define REMOTECHANGENOTIFIER_H

#include <QObject>
#include <QTimer>
#include <QDeclarativeView>
#include <KTextEditor/View>

namespace QInfinity {
    class User;
}

// This class is used to draw fancy widgets in the editor window when a
// remote user changes text
class RemoteChangeNotifier : public QObject
{
public:
    static void addNotificationWidget(KTextEditor::View* view, KTextEditor::Cursor cursor, const QInfinity::User* user);
    // TODO do those use enough memory to be worth freeing temporarily? Each one might eventually be re-used
    // if the user writes more text, so it's not really a leak.
    static QMap< QPair<KTextEditor::View*, QString>, QWidget* > existingWidgets;
};

class NotifierWidget : public QDeclarativeView {
public:
    NotifierWidget(const QUrl& source, QWidget* parent = 0);
    QTimer* closeTimer;
};
#endif // REMOTECHANGENOTIFIER_H
