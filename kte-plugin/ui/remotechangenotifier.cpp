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
#include <common/utils.h>
#include <libqinfinity/user.h>
#include <QLabel>
#include <QPaintEvent>

#include <QGraphicsItem>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <QTimer>
#include <kstandarddirs.h>

QMap< QPair<KTextEditor::View*, QString>, QWidget* > RemoteChangeNotifier::existingWidgets;

NotifierWidget::NotifierWidget(const QUrl& source, QWidget* parent)
    : QDeclarativeView(source, parent)
    , m_closeTimer(new QTimer(parent))
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

void RemoteChangeNotifier::addNotificationWidget(KTextEditor::View* view, KTextEditor::Cursor cursor, const QInfinity::User* user)
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
            kWarning() << "Errors occured while loading" << src;
            return;
        }

        useWidget = widget;
        existingWidgets[key] = widget;
    }

    NotifierWidget* notifierWidget = static_cast<NotifierWidget*>(useWidget);
    notifierWidget->rootObject()->setProperty("username", user->name());
    notifierWidget->rootObject()->setProperty("widgetcolor", ColorHelper::colorForUsername(user->name()).name());
    QObject* hideAnimation = notifierWidget->rootObject()->findChild<QObject*>("hideAnimation");
    // restart animation
    QMetaObject::invokeMethod(hideAnimation, "restart");
    // reset widget opacity + position
    QMetaObject::invokeMethod(notifierWidget->rootObject(), "reset");
    notifierWidget->startCloseTimer();

    // update the position now, and when the user scrolls
    QObject::connect(view, SIGNAL(verticalScrollPositionChanged(KTextEditor::View*,KTextEditor::Cursor)),
                     notifierWidget, SLOT(moveWidget(KTextEditor::View*,KTextEditor::Cursor)));
    QObject::connect(view, SIGNAL(horizontalScrollPositionChanged(KTextEditor::View*)),
                     notifierWidget, SLOT(moveWidget(KTextEditor::View*,KTextEditor::Cursor)));
    notifierWidget->setCursorPosition(cursor);
    notifierWidget->moveWidget(view);
}

void NotifierWidget::startCloseTimer()
{
    m_closeTimer->start();
}

void NotifierWidget::moveWidget(KTextEditor::View* view, KTextEditor::Cursor /*cursor*/)
{
    // The cursor we get as an argument is not the cursor we want to move to!
    // It's set by the view to where the user scrolled to.
    // use KTE api to calculate position
    // TODO handle out-of-view changes nicely, by pointing the arrow up or down
    QPoint pos = mapToParent(view->cursorToCoordinate(m_position));
    pos.setY(pos.y() + view->fontMetrics().height()*0.8 - y());
    pos.setX(pos.x() - 15 - x());
    QPoint pos2 = mapToParent(pos);
    move(qMax(10, pos2.x() - x()), pos2.y() - y());
    show();
}
