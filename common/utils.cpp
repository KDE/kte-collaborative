/* This file is part of the KDE libraries
   Copyright (C) 2013 Sven Brauch <svenbrauch@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include "common/utils.h"
#include "selecteditorwidget.h"

#include <libqinfinity/explorerequest.h>
#include <ktexteditor/configinterface.h>

#include <QStringList>
#include <KTextEditor/View>
#include <KConfigGroup>
#include <KConfig>
#include <KStandardDirs>
#include <KRun>
#include <KToolInvocation>

using QInfinity::ExploreRequest;

bool tryOpenDocument(const KUrl& url)
{
    KUrl dir = url.upUrl();
    KConfig config("ktecollaborative");
    KConfigGroup group = config.group("applications");
    // We do not set a default value here, so the dialog is always
    // displayed the first time the user uses the feature.
    QString command = group.readEntry("editor", "");
    if ( command.isEmpty() ) {
        return false;
    }

    command = command.replace("%u", url.url());
    command = command.replace("%d", dir.url());
    command = command.replace("%h", url.host() + ( url.port() ? (":" + QString::number(url.port())) : QString()));
    QString executable = command.split(' ').first();
    QString arguments = QStringList(command.split(' ').mid(1, -1)).join(" ");
    QString executablePath = KStandardDirs::findExe(executable);
    if ( executablePath.isEmpty() ) {
        return false;
    }
    return KRun::runCommand(executablePath + " " + arguments, 0);
}

bool tryOpenDocumentWithDialog(const KUrl& url)
{
    while ( ! tryOpenDocument(url) ) {
        SelectEditorDialog dlg;
        if ( ! dlg.exec() ) {
            return false;
        }
    }
    return true;
}

bool ensureNotifierModuleLoaded()
{
    KStandardDirs d;
    QString desktopPath = d.findResource("services", "infinotenotifier.desktop");
    return KToolInvocation::startServiceByDesktopPath(desktopPath) == 0;
}

QString getUserName()
{
    QString user;
#ifdef Q_OS_WIN
    user = qgetenv("USERNAME");
#else
    user = qgetenv("USER");
#endif
    if ( user.length() > 0 ) {
        user[0] = user[0].toUpper();
    }
    return user;
}

IterLookupHelper::IterLookupHelper(QString lookupPath, const QInfinity::Browser* browser)
        : QObject()
        , m_browser(browser)
        , m_currentIter(*m_browser)
        , m_wasSuccessful(false)
{
    // remove starting and trailing slash
    if ( lookupPath.startsWith('/') ) {
        lookupPath = lookupPath.mid(1);
    }
    foreach ( const QString& component, lookupPath.split('/').toVector() ) {
        m_remainingComponents.prepend(component);
    }
    kDebug() << "finding iter for" << m_remainingComponents;
};

void IterLookupHelper::setDeleteOnFinish(bool deleteOnFinish)
{
    if ( deleteOnFinish ) {
        connect(this, SIGNAL(done(QInfinity::BrowserIter)), this, SLOT(deleteLater()));
    }
    else {
        disconnect(this, SIGNAL(done(QInfinity::BrowserIter)), this, SLOT(deleteLater()));
    }
}

bool IterLookupHelper::success() const
{
    return m_wasSuccessful;
}

void IterLookupHelper::explore(QInfinity::BrowserIter directory)
{
    if ( ! directory.isExplored() ) {
        kDebug() << "exploring iter";
        ExploreRequest* request = directory.explore();
        m_currentIter = directory;
        connect(request, SIGNAL(finished(ExploreRequest*)), this, SLOT(directoryExplored()));
    }
    else {
        directoryExplored();
    }
};

QInfinity::BrowserIter IterLookupHelper::result() const
{
    return m_currentIter;
}

void IterLookupHelper::setExploreResult(bool exploreResult)
{
    if ( exploreResult ) {
        connect(this, SIGNAL(done(QInfinity::BrowserIter)), this, SLOT(exploreIfDirectory(QInfinity::BrowserIter)));
    }
    else {
        disconnect(this, SIGNAL(done(QInfinity::BrowserIter)), this, SLOT(exploreIfDirectory(QInfinity::BrowserIter)));
    }
}

void IterLookupHelper::exploreIfDirectory(QInfinity::BrowserIter iter)
{
    if ( iter.isDirectory() && ! iter.isExplored() ) {
        iter.explore();
    }
}

void IterLookupHelper::directoryExplored()
{
    kDebug() << "directory explored";
    QString findEntry = m_remainingComponents.pop();
    kDebug() << "finding:" << findEntry << " -- remaining:" << m_remainingComponents;
    if ( findEntry.isEmpty() ) {
        // the path is a directory; return the directory iter instead of a child
        m_wasSuccessful = true;
        emit done(m_currentIter);
        return;
    }
    bool hasChildren = m_currentIter.child();
    if ( ! hasChildren ) {
        emit failed();
        return;
    }

    bool found = false;
    do {
        kDebug() << m_currentIter.name();
        if ( m_currentIter.name() == findEntry ) {
            found = true;
            break;
        }
    } while ( m_currentIter.next() );

    // no entries remain and the item was found
    bool fullyFound = found && m_remainingComponents.isEmpty();
    // just an empty entry remains and the current item is not a directory
    bool directoryFound = m_remainingComponents.size() == 1 && m_remainingComponents.first().isEmpty()
                          && ! m_currentIter.isDirectory();
    if ( fullyFound || directoryFound ) {
        m_wasSuccessful = true;
        emit done(m_currentIter);
        return;
    }
    else if ( found ) {
        return explore(m_currentIter);
    }
    kWarning() << "explore failed!";
    emit failed();
};

QColor ColorHelper::colorForUsername(const QString& username, unsigned char sat,
                                     unsigned char brightness, const QMap<QString, QColor>& usedColors)
{
    if ( usedColors.contains(username) ) {
        return usedColors[username];
    }
    const uint hash = qHash(username);
    uint hue = ((hash % 19) * 4129) % 360;
    const int minDistance = 30;
    // Find the color which is closest to the choosen color, and retry if it's too close.
    uint closestDistance;
    for ( int i = 0; i < 360 / minDistance; i++ ) {
        closestDistance = 360;
        foreach ( const QColor& color, usedColors ) {
            const int chue = color.hsvHue();
            const uint distance = abs(chue - hue) <= 180 ? abs(chue - hue) : 360 - abs(chue - hue);
            Q_ASSERT(distance <= 180); // if not, my maths is broken
            closestDistance = qMin(distance, closestDistance);
        }
        if ( closestDistance <= minDistance ) {
            // Go one point off the oposite of the color wheel,
            // for a good chance for good contrast
            hue += minDistance * (360 / minDistance / 2 - 1);
            hue = hue % 360;
        }
        else {
            break;
        }
    }
    if ( closestDistance < minDistance ) {
        // Still no luck -- most colors are used apparently.
        // Try changing brightness and hope that works.
        brightness = brightness > 128 ? brightness - 40 : brightness + 40;
    }
    const uint val = qMin<int>(brightness + ((hash % 3741) * 17) % 20, 255);
    QColor color = QColor::fromHsv(hue, sat, val);
    while ( y(color) < qMin<int>(brightness + ((hash % 3011) * 13) % 20 - 10, 215) ) {
        color = color.lighter(115);
    }
    return color;
}

QColor ColorHelper::colorForUsername(const QString& username, const KTextEditor::View* view,
                                     const QMap<QString, QColor>& usedColors)
{
    if ( usedColors.contains(username) ) {
        return usedColors[username];
    }
    // Try to find a brightness which has good contrast to the text
    short backgroundBrightness = 195;
    KTextEditor::ConfigInterface* iface = qobject_cast<KTextEditor::ConfigInterface*>(view);
    if ( iface ) {
        backgroundBrightness = qMin(y(iface->configValue("background-color").value<QColor>()), 255);
        if ( backgroundBrightness < 60 ) {
            // don't make it too dark, it's hard to see otherwise
            backgroundBrightness += 10;
        }
        else if ( backgroundBrightness > 225 ) {
            backgroundBrightness -= 20;
        }
        else if ( backgroundBrightness > 200 ) {
            backgroundBrightness -= 10;
        }
    }
    // Read the user-configured saturation
    KConfig config(QLatin1String("ktecollaborative"));
    KConfigGroup group = config.group("colors");
    int saturation = group.readEntry("saturation", 185);
    return colorForUsername(username, saturation, backgroundBrightness, usedColors);
}

int ColorHelper::y(const QColor& color)
{
    return 0.299*color.red() + 0.587*color.green() + 0.114*color.blue();
}
