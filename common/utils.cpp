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

#include <QStringList>

IterLookupHelper::IterLookupHelper(QString lookupPath, QInfinity::Browser* browser)
        : QObject()
        , m_browser(browser)
        , m_currentIter(*m_browser)
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

void IterLookupHelper::finished_cb(InfcNodeRequest* request, void* user_data)
{
    kDebug() << "explore request finished";
    static_cast<IterLookupHelper*>(user_data)->directoryExplored();
}

void IterLookupHelper::explore(QInfinity::BrowserIter directory)
{
    if ( ! directory.isExplored() ) {
        kDebug() << "exploring iter";
        InfcExploreRequest* request = directory.explore();
        m_currentIter = directory;
        g_signal_connect_after(request, "finished",
                                G_CALLBACK(IterLookupHelper::finished_cb), (void*) this);
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

    if ( m_remainingComponents.isEmpty() ) {
        // no directories remain
        emit done(m_currentIter);
        return;
    }
    else if ( found ) {
        return explore(m_currentIter);
    }
    kWarning() << "explore failed!";
    emit failed();
};
