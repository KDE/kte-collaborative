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
#include <kstandarddirs.h>

QMap< QPair<KTextEditor::View*, QString>, QWidget* > RemoteChangeNotifier::existingWidgets;

NotifierWidget::NotifierWidget(const QUrl& source, QWidget* parent)
    : QDeclarativeView(source, parent)
    , closeTimer(new QTimer(parent))
{
    closeTimer->setSingleShot(true);
    closeTimer->setInterval(3000);
    connect(closeTimer, SIGNAL(timeout()), this, SLOT(hide()));
}

// TODO use user's color instead -- or maybe not?
// After all, this whole "select your color" stuff is of questionable use, and this works nicely.
QColor colorForUsername(QString username) {
    unsigned int hash = 0;
    for ( int i = 0; i < username.size(); i++ ) {
        hash += (17*i%31+1)*username.at(i).toAscii();
    }
    return QColor::fromHsv(hash % 359, 255, 255);
};

void RemoteChangeNotifier::addNotificationWidget(KTextEditor::View* view, KTextEditor::Cursor cursor, const QString& username)
{
    QWidget* useWidget = 0;
    KStandardDirs d;
    QUrl src = QUrl(d.locate("data", "kte-kobby/ui/notifywidget.qml"));
    bool added = false;

    QPair< KTextEditor::View*, QString > key = QPair<KTextEditor::View*, QString>(view, username);
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
            return;
        }

        useWidget = widget;
        existingWidgets[key] = widget;
        added = true;
    }

    // TODO use + set correct color
    NotifierWidget* notifierWidget = static_cast<NotifierWidget*>(useWidget);
    notifierWidget->rootObject()->setProperty("username", username);
    notifierWidget->rootObject()->setProperty("widgetcolor", colorForUsername(username).name());
    QObject* hideAnimation = notifierWidget->rootObject()->findChild<QObject*>("hideAnimation");
    // restart animation
    QMetaObject::invokeMethod(hideAnimation, "restart");
    // reset widget opacity + position
    QMetaObject::invokeMethod(notifierWidget->rootObject(), "reset");
    notifierWidget->closeTimer->start();

    // use KTE api to calculate position
    // TODO handle out-of-view changes nicely, by pointing the arrow up or down
    // TODO support scrolling while the widget is visible
    QPoint pos = useWidget->mapToParent(view->cursorToCoordinate(cursor));
    pos.setY(pos.y() + view->fontMetrics().height()*0.8 - useWidget->y());
    pos.setX(pos.x() - 15 - useWidget->x());
    QPoint pos2 = useWidget->mapToParent(pos);
    useWidget->move(qMax(10, pos2.x() - useWidget->x()), pos2.y() - useWidget->y());
    useWidget->show();
    kDebug() << "widget size:" << useWidget->size() << useWidget->isVisible() << pos << useWidget->pos();
}
