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
#include "statusoverlay.h"
#include "version.h"

#include <libqinfinity/session.h>

#include <KStandardDirs>
#include <KDebug>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KLocalizedString>
#include <qdeclarativeerror.h>
#include <qdeclarativeitem.h>

StatusOverlay::StatusOverlay(KTextEditor::View* parent)
    : QDeclarativeView(QUrl(KStandardDirs().locate("data", "kte-kobby/ui/overlay.qml")), parent)
    , m_view(parent)
{
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::transparent);
    setPalette(p);
    setBackgroundRole(QPalette::Window);
    setBackgroundBrush(QBrush(QColor(0, 0, 0, 0)));
    setAutoFillBackground(true);
    if ( ! rootObject() ) {
        kWarning() << "error creating overlay";
        return;
    }
    kDebug() << "view size:" << m_view->size();
    m_view->installEventFilter(this);
    resizeToView();
    displayText(i18n("Initializing..."));

    QObject* textWidget = rootObject()->findChild<QObject*>("subtitle");
    QString subtitle = i18n("KTextEditor collaborative editing plugin version %1",
                            QString(KTECOLLAB_VERSION_STRING)) + "<br>" +
                       i18n("using libinfinity version %1", QString(LIBINFINITY_VERSION));
    textWidget->setProperty("text", subtitle);
}

void StatusOverlay::progress(double percentage)
{
    setProgressBar(percentage);
    displayText(i18nc("%1 is a progress percentage", "Synchronizing document... %1%", static_cast<int>(percentage*100)));
    repaint();
}

void StatusOverlay::setProgressBar(double percentage)
{
    QObject* progressbar = rootObject()->findChild<QObject*>("progressBar");
    progressbar->setProperty("progress", percentage);
}

void StatusOverlay::loadStateChanged(Document* , Document::LoadState state)
{
    if ( state == Document::Joining ) {
        setProgressBar(1.0);
        displayText(i18n("Joining session..."));
    }
    if ( state == Document::Complete ) {
        displayText(i18n("Done."));
        hide();
    }
}

void StatusOverlay::connectionStatusChanged(Connection* , QInfinity::XmlConnection::Status status)
{
    if ( status == QInfinity::XmlConnection::Opening ) {
        displayText(i18n("Connecting..."));
    }
    if ( status == QInfinity::XmlConnection::Open ) {
        displayText(i18n("Synchronizing document..."));
    }
}

void StatusOverlay::displayText(const QString& text)
{
    QObject* textWidget = rootObject()->findChild<QObject*>("text");
    textWidget->setProperty("text", text);
}

void StatusOverlay::resizeToView()
{
    resize(m_view->width(), m_view->height());
    qobject_cast<QDeclarativeItem*>(rootObject())->setSize(size());
}

bool StatusOverlay::eventFilter(QObject* watched, QEvent* e)
{
    if ( watched == m_view && e->type() == QEvent::Resize ) {
        resizeToView();
    }
    return QDeclarativeView::eventFilter(watched, e);
}

#include "statusoverlay.moc"
