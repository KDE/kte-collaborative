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

#include "remotechangenotifier.h"
#include <QLabel>
#include <QPaintEvent>

#include <QGraphicsItem>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <QTimer>

QMap< QPair<KTextEditor::View*, QString>, QWidget* > RemoteChangeNotifier::existingWidgets;

void RemoteChangeNotifier::addNotificationWidget(KTextEditor::View* view, KTextEditor::Cursor cursor, const QString& username)
{
    QWidget* useWidget = 0;
    QUrl src = QUrl("/home/sven/Projekte/kde/kobby/common/ui/notifywidget.qml");

    QPair< KTextEditor::View*, QString > key = QPair<KTextEditor::View*, QString>(view, username);
    if ( existingWidgets.contains(key) ) {
        // move an existing widget
        useWidget = existingWidgets[key];
        // set the source again to re-load the QML file + animations
        static_cast<QDeclarativeView*>(useWidget)->setSource(src);
    }
    if ( ! useWidget ) {
        // create a new widget
        // this is for getting semi-transparent widgets
        QDeclarativeView* widget = new QDeclarativeView(view);
        QPalette p = widget->palette();
        p.setColor( QPalette::Window, Qt::transparent );
        widget->setPalette( p );
        widget->setBackgroundRole( QPalette::Window );
        widget->setBackgroundBrush(QBrush(QColor(0, 0, 0, 0)));
        widget->setAutoFillBackground( true );

        // load the QML file which draws the user label
        widget->setSource(src);
        if ( ! widget->rootContext() ) {
            return;
        }


        useWidget = widget;
        existingWidgets[key] = widget;
    }

    QTimer::singleShot(3000, useWidget, SLOT(hide()));
    kDebug() << "parent:" << useWidget->parent() << view;

    // TODO use + set correct color
    static_cast<QDeclarativeView*>(useWidget)->rootObject()->setProperty("username", username);

    // use KTE api to calculate position
    QPoint pos = useWidget->mapToParent(view->cursorToCoordinate(cursor));
    pos.setY(pos.y() + view->fontMetrics().height()*0.8 - useWidget->y());
    pos.setX(pos.x() - 15 - useWidget->x());
    QPoint pos2 = useWidget->mapToParent(pos);
    useWidget->move(pos2.x() - useWidget->x(), pos2.y() - useWidget->y());
    useWidget->show();
    kDebug() << "widget size:" << useWidget->size() << useWidget->isVisible() << pos << useWidget->pos();
}
