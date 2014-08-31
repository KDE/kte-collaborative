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

#ifndef STATUSOVERLAY_H
#define STATUSOVERLAY_H

#include <QDeclarativeView>
#include <libqinfinity/xmlconnection.h>

#include "common/document.h"

namespace Kobby {
    class Connection;
}
namespace KTextEditor {
    class View;
}
using Kobby::Document;
using Kobby::Connection;

/**
 * @brief This class provides the overlay which is displayed when a document is loading.
 */
class StatusOverlay : public QDeclarativeView
{
Q_OBJECT
public:
    /**
     * @brief Constructs a new overlay, but does not show it. Size of the given @p parent is tracked.
     */
    explicit StatusOverlay(KTextEditor::View* parent);

    /**
     * @brief Event filter for tracking the parent's size
     */
    virtual bool eventFilter(QObject* watched, QEvent* e);

    /**
     * @brief Display @p text in the text box over the progress bar.
     */
    void displayText(const QString& text);

public slots:
    /**
     * @brief Called when synchronization progresses. Updates the progress bar and text display.
     * @param percentage Percentage completed, from 0.0 to 1.0
     */
    void progress(double percentage);
    void loadStateChanged(Document*,Document::LoadState);
    void connectionStatusChanged(Connection*,QInfinity::XmlConnection::Status);

private:
    /**
     * @brief Resizes the overlay to the parent view's size
     */
    void resizeToView();
    /**
     * @brief Updates the displayed progressbar with a given value.
     * @param percentage 0.0 = empty, 1.0 = full
     */
    void setProgressBar(double percentage);

private:
    KTextEditor::View* m_view;
    /// Timer to prevent too frequent updates. On my machine, a redraw takes about
    /// 5ms, so make sure we don't repaint more than 10 times a second in any case.
    QTime m_maxUpdateRateTimer;
};

#endif // STATUSOVERLAY_H
