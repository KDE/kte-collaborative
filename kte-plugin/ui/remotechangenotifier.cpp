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

#include "kobbypluginview.h"
#include "common/utils.h"

#include <libqinfinity/user.h>

#include <KTextEditor/Document>
#include <KStandardDirs>

#include <QGraphicsItem>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <QPaintEvent>
#include <QLabel>
#include <QTimer>

QMap< QPair<KTextEditor::View*, QString>, QWidget* > RemoteChangeNotifier::existingWidgets;

NotifierWidget::NotifierWidget(const QUrl& source, QWidget* parent)
    : QDeclarativeView(source, parent)
    , m_closeTimer(new QTimer(parent))
    , m_forceUpdate(false)
{
    m_closeTimer->setSingleShot(true);
    m_closeTimer->setInterval(3000);
    connect(m_closeTimer, SIGNAL(timeout()), this, SLOT(hide()));
}

bool NotifierWidget::event(QEvent* event)
{
    if ( event->type() == QEvent::MouseButtonPress ) {
        hide();
        event->ignore();
        return false;
    }
    return QGraphicsView::event(event);
}

void RemoteChangeNotifier::addNotificationWidget(KTextEditor::View* view, KTextEditor::Cursor cursor,
                                                 const QInfinity::User* user, const QColor& color)
{
    if ( ! view ) {
        return;
    }
    QWidget* useWidget = 0;
    KStandardDirs d;
    QUrl src = QUrl(d.locate("data", "kte-kobby/ui/notifywidget.qml"));

    QPair< KTextEditor::View*, QString > key = QPair<KTextEditor::View*, QString>(view, user->name());
    if ( existingWidgets.contains(key) ) {
        // move an existing widget
        useWidget = existingWidgets[key];
    }
    if ( ! useWidget ) {
        // create a new widget
        // this is for getting semi-transparent widgets
        // load the QML file which draws the user label
        QDeclarativeView* widget = new NotifierWidget(src, view);
        QPalette p = widget->palette();
        p.setColor( QPalette::Window, Qt::transparent );
        widget->setPalette( p );
        widget->setBackgroundRole( QPalette::Window );
        widget->setBackgroundBrush(QBrush(QColor(0, 0, 0, 0)));
        widget->setAutoFillBackground( true );

        // check if loading the QML file was successful, otherwise abort
        if ( ! widget->rootContext() ) {
            kWarning() << "Errors occurred while loading" << src;
            return;
        }

        useWidget = widget;
        existingWidgets[key] = widget;
    }

    NotifierWidget* notifierWidget = static_cast<NotifierWidget*>(useWidget);
    notifierWidget->rootObject()->setProperty("username", user->name());
    notifierWidget->rootObject()->setProperty("widgetcolor", color.name());
    notifierWidget->rootObject()->setProperty("brightness", 1 - ColorHelper::y(color) / 255.0);
    QObject* hideAnimation = notifierWidget->rootObject()->findChild<QObject*>("hideAnimation");
    // restart animation
    QMetaObject::invokeMethod(hideAnimation, "restart");
    notifierWidget->startCloseTimer();

    // update the position now, and when the user scrolls
    QObject::connect(view, SIGNAL(verticalScrollPositionChanged(KTextEditor::View*,KTextEditor::Cursor)),
                     notifierWidget, SLOT(moveWidget(KTextEditor::View*)), Qt::UniqueConnection);
    QObject::connect(view, SIGNAL(horizontalScrollPositionChanged(KTextEditor::View*)),
                     notifierWidget, SLOT(moveWidget(KTextEditor::View*)), Qt::UniqueConnection);
    notifierWidget->setCursorPosition(cursor);
    notifierWidget->forceUpdate();
    notifierWidget->moveWidget(view);
    // reset widget opacity + position
    QMetaObject::invokeMethod(notifierWidget->rootObject(), "reset");
    notifierWidget->show();
}

void NotifierWidget::startCloseTimer()
{
    m_closeTimer->start();
}

void NotifierWidget::moveWidget(KTextEditor::View* view)
{
    if ( ! m_forceUpdate && ! isVisible() ) {
        return;
    }
    // use KTE api to calculate position
    const QPoint rawPos = view->cursorToCoordinate(m_position);
    if ( rawPos == QPoint(-1, -1) ) {
        // position is above or below the view
        rootObject()->setProperty("outsideView", true);
        if ( KTextEditor::CoordinatesToCursorInterface* iface = qobject_cast<KTextEditor::CoordinatesToCursorInterface*>(view)) {
            const KTextEditor::Cursor topLeft = iface->coordinatesToCursor(QPoint(0, 0));
            if ( topLeft.line() < m_position.line() ) {
                // position is below the view
                int bottom = view->height() - height();
                if ( QWidget* statusBar = view->findChild<KobbyStatusBar*>() ) {
                    bottom -= statusBar->height();
                }
                move(0, bottom);
            }
            else {
                // position is above the view
                move(0, 0);
            }
        }
        else {
            // interface is not supported; just hide the widget
            hide();
            return;
        }
    }
    else {
        rootObject()->setProperty("outsideView", false);
        QPoint pos = mapToParent(rawPos);
        pos.setY(pos.y() + view->fontMetrics().height()*0.8 - y());
        pos.setX(pos.x() - 15 - x());
        QPoint pos2 = mapToParent(pos);
        move(qMax(10, pos2.x() - x()), pos2.y() - y());
    }
}
