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

#ifndef KOBBY_UTILS_H
#define KOBBY_UTILS_H
#include "kobbycommon_export.h"

#include <QObject>
#include <QStack>
#include <KDebug>

#include <glib.h>

#include <libqinfinity/browser.h>
#include <libqinfinity/browseriter.h>

// Helper class for finding the BrowserIter for a directory.
// libinfinity works with documents (or directories) only as "iters",
// which are basically iterators of a tree model which represents
// the file structure. When asking about a document, e.g. when you want to
// subscribe to it, you need to provide this iterator; the URL won't be accepted.
// Since getting the contents of a directory
// requires asking the server and waiting for a reply sometimes,
// this class provides a convenient way to retrieve the iterator for
// a given path.
// Connect to the done() signal to get notified when the iter has been found.
// Exploration results are cached by the underlying library, so this
// operation is fast when it has been done before for the given path (excluding
// the last entry), and network-slow if it has not (might need to display
// a busy indicator while it's running).
class KOBBYCOMMON_EXPORT IterLookupHelper : public QObject {
Q_OBJECT
public:
    IterLookupHelper(QString lookupPath, QInfinity::Browser* browser);
    static void finished_cb( InfcNodeRequest* request, void* user_data );

    inline void begin() {
        explore(m_currentIter);
    };

signals:
    void done(QInfinity::BrowserIter found);
    void failed();

protected:
    void directoryExplored();
    void explore(QInfinity::BrowserIter directory);

    QStack<QString> m_remainingComponents;
    QInfinity::Browser* m_browser;
    QInfinity::BrowserIter m_currentIter;
};

#endif
