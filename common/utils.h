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
#include "ktecollaborativecommon_export.h"

#include <QObject>
#include <QStack>
#include <QTimer>
#include <QColor>
#include <QDebug>
#include <QMap>

#include <libqinfinity/browser.h>
#include <libqinfinity/browseriter.h>

namespace KTextEditor {
class View;
}

/**
  * @brief Try to open the given document based on the current configuration in KConfig
  *
  * @param url The URL to open
  * @return bool true if the command could be executed, false otherwise
  */
KTECOLLABORATIVECOMMON_EXPORT bool tryOpenDocument(const QUrl& url);
KTECOLLABORATIVECOMMON_EXPORT bool tryOpenDocumentWithDialog(const QUrl& url);

KTECOLLABORATIVECOMMON_EXPORT bool ensureNotifierModuleLoaded();
KTECOLLABORATIVECOMMON_EXPORT QString getUserName();


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
class KTECOLLABORATIVECOMMON_EXPORT IterLookupHelper : public QObject {
Q_OBJECT
public:
    IterLookupHelper(QString lookupPath, const QInfinity::Browser* browser);

    inline void beginLater() {
        QTimer::singleShot(0, this, SLOT(begin()));
    };
    QInfinity::BrowserIter result() const;
    bool success() const;
    void setDeleteOnFinish(bool deleteOnFinish = true);
    void setExploreResult(bool exploreResult = true);

signals:
    void done(QInfinity::BrowserIter found);
    void failed();

public slots:
    void begin() {
        qDebug() << "beginning explore";
        explore(m_currentIter);
    };
    void directoryExplored();
    void exploreIfDirectory(QInfinity::BrowserIter);

protected:
    void explore(QInfinity::BrowserIter directory);

    QStack<QString> m_remainingComponents;
    const QInfinity::Browser* m_browser;
    QInfinity::BrowserIter m_currentIter;
    bool m_wasSuccessful;
};

// Helper class for dealing with colors.
class KTECOLLABORATIVECOMMON_EXPORT ColorHelper {
public:
    /**
     * @brief YUV "Y" value of the given color, between 255 (very light) and 0 (very dark).
     */
    static int y(const QColor& color);

    /**
     * @brief Generate a color depending on a given username.
     * This is used for example for the background colors, and the popup widgets.
     * TODO: Cache this.
     * @param username The username. The same username will always yield the same color.
     * @param sat Hint on how saturated the color should be; 255 = very colorful, 0 = black+white
     * @param brightness Hint on how bright the color should be; 255 = white, 0 = black
     * @return QColor The resulting color.
     */
    static QColor colorForUsername(const QString& username, unsigned char sat = 180, unsigned char brightness = 180,
                                   const QMap<QString, QColor>& usedColors = (QMap<QString, QColor>()));

    /**
     * @brief Like colorForUsername(QString, uchar, uchar), but determines params automatically
     * It will read the saturation from KConfig (user configurable), and the brightness from the view.
     * @param username The username to generate a color for
     * @param view The view in which the color should be used as a background. May be zero, then brightness will be guessed.
     * @return QColor The resulting color.
     */
    static QColor colorForUsername(const QString& username, const KTextEditor::View* view,
                                   const QMap<QString, QColor>& usedColors = (QMap<QString, QColor>()));
};

#endif
