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

#include <QStringList>

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
