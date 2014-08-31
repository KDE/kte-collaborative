/*
 * This file is part of kobby
 * Copyright 2013  Sven Brauch <svenbrauch@gmail.com>
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

#ifndef DOCUMENTCHANGETRACKER_H
#define DOCUMENTCHANGETRACKER_H

#include <QObject>

#include <KTextEditor/Range>

namespace KTextEditor {
    class Document;
    class MovingInterface;
class MovingRange;
}

namespace QInfinity {
    class User;
}

class ManagedDocument;

/**
 * @brief Class for tracking changes to a collaborative document.
 * Its current purpose is to take care of the colorful background highlighting.
 */
class DocumentChangeTracker : public QObject {
Q_OBJECT
public:
    DocumentChangeTracker(ManagedDocument* const document);

public slots:
    /**
     * @brief Should be invoked when anyone (you or a remote user) changes the document's text.
     *
     * @param range The range of the text which was changed
     * @param user The user who changed the text
     * @param removal true if the text was removed, else false
     */
    void userChangedText(const KTextEditor::Range& range, QInfinity::User* user, bool removal);

    /**
     * @brief Sets up the signals notifying this class about changes to the text after synchronization begins.
     */
    void setupSignals();

    /**
     * @brief Clears all existing highlights.
     */
    void clearHighlight();

    /**
     * @brief A map of colors which are being used already.
     */
    const QMap<QString, QColor>& usedColors() const;

    /**
     * @brief Get the user name of the person that last changed "position".
     *
     * @param position The position to get the user name for
     * @return QString readable name of the user who last changed this text
     */
    QString userForCursor(const KTextEditor::Cursor& position) const;

signals:
    /**
     * @brief Emitted when the used colors change in some way.
     */
    void colorTableChanged();

private:
    /**
     * @brief Finds empty ranges in m_ranges and deletes them.
     */
    void cleanupRanges();

    void splitRangeForInsertion(KTextEditor::MovingRange* existing, const KTextEditor::Range& splitFor);

    KTextEditor::MovingRange* rangeAt(const KTextEditor::Range& range);

    /**
     * @brief Adds a range to the highlighted ranges.
     *
     * @param range The raw range to start tracking
     * @param color The color to use for highlighting
     * @param bySelf should be true if text was inserted by the local user
     * @return The range which was created
     */
    KTextEditor::MovingRange* addHighlightedRange(const QString& name, const KTextEditor::Range& range, const QColor& color);

    ManagedDocument* const m_document;
    KTextEditor::Document* kDocument() const;
    KTextEditor::MovingInterface* m_iface;
    inline KTextEditor::MovingInterface* iface() const { return m_iface; };
    QList<KTextEditor::MovingRange*> m_ranges;
    // Maps user names to colors
    QMap<QString, QColor> m_existingColors;
};

#endif // DOCUMENTCHANGETRACKER_H
