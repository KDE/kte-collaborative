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
#include <libqinfinity/explorerequest.h>
#include <ktexteditor/configinterface.h>

#include <QStringList>
#include <KTextEditor/View>
#include <KConfigGroup>
#include <kconfig.h>

using QInfinity::ExploreRequest;

IterLookupHelper::IterLookupHelper(QString lookupPath, QInfinity::Browser* browser)
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

QColor ColorHelper::colorForUsername(const QString& username, const unsigned char sat,
                                     const unsigned char brightness)
{
    const uint hash = qHash(username);
    const uint hue = ((hash % 19) * 4129) % 360;
    const uint val = qMin<int>(brightness + ((hash % 3741) * 17) % 35, 255);
    QColor color = QColor::fromHsv(hue, sat, val);
    while ( y(color) < qMin<int>(brightness + ((hash % 3011) * 13) % 20 - 10, 225) ) {
        color = color.lighter(115);
    }
    return color;
}

QColor ColorHelper::colorForUsername(const QString& username, const KTextEditor::View* view)
{
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
    return colorForUsername(username, saturation, backgroundBrightness);
}

int ColorHelper::y(const QColor& color)
{
    return 0.299*color.red() + 0.587*color.green() + 0.114*color.blue();
}
