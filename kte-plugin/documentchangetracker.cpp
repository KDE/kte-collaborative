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

#include "documentchangetracker.h"
#include "manageddocument.h"
#include <common/utils.h>

#include <ktexteditor/movinginterface.h>

DocumentChangeTracker::DocumentChangeTracker(ManagedDocument* const document)
    : QObject(document)
    , m_document(document)
    , m_iface(qobject_cast<KTextEditor::MovingInterface*>(document->document()))
{
    kDebug() << "change tracker created for" << document->document()->url() << "moving interface:" << m_iface;
    connect(m_document, SIGNAL(synchronizationBegins(ManagedDocument*)),
            this, SLOT(setupSignals()));
}

void DocumentChangeTracker::setupSignals()
{
    connect(m_document->textBuffer(), SIGNAL(localChangedText(KTextEditor::Range,QInfinity::User*,bool)),
            this, SLOT(userChangedText(KTextEditor::Range,QInfinity::User*,bool)));
    connect(m_document->textBuffer(), SIGNAL(remoteChangedText(KTextEditor::Range,QInfinity::User*,bool)),
            this, SLOT(userChangedText(KTextEditor::Range,QInfinity::User*,bool)));
}

KTextEditor::Document* DocumentChangeTracker::kDocument() const
{
    return m_document->document();
}

void DocumentChangeTracker::cleanupRanges()
{
    // TODO: Could join ranges in some cases. Worth it?
    for ( int i = 0; i < m_ranges.size(); i++ ) {
        KTextEditor::MovingRange* r = m_ranges.at(i);
        if ( r->isEmpty() ) {
            m_ranges.removeAt(i);
            delete r;
            i -= 1;
        }
    }
}

void DocumentChangeTracker::addHighlightedRange(const KTextEditor::Range& range, const QColor& color)
{
    // We allow empty ranges here, and invalidate them ourselves on the next insertion.
    KTextEditor::MovingRange* r = iface()->newMovingRange(range, KTextEditor::MovingRange::DoNotExpand,
                                                          KTextEditor::MovingRange::AllowEmpty);
    KTextEditor::Attribute::Ptr attrib(new KTextEditor::Attribute);
    attrib->setBackground(color);
    r->setAttribute(attrib);
    m_ranges << r;
}

void DocumentChangeTracker::userChangedText(const KTextEditor::Range& range, QInfinity::User* user, bool removal)
{
    if ( ! iface() ) {
        return;
    }
    if ( removal ) {
        // Nothing to do for removals, ranges will shrink automatically
        return;
    }
    cleanupRanges();
    const int startLine = range.start().line();
    const int endLine = range.end().line();
    if ( m_document->document()->text(range, false) == QLatin1String("\n") ) {
        // Special case optimization: just a newline was inserted; nothing needs to be done.
        return;
    }
    else if ( startLine != endLine ) {
        // If the range spans multiple lines, add multiple moving ranges to avoid
        // highlighting the newlines.
        // TODO maybe we want this behaviour to be configurable?
        for ( int line = startLine; line <= endLine; line++ ) {
            const int lineSize = m_document->document()->lineLength(line);
            if ( lineSize == 0 ) {
                // do not create empty highlight ranges
                continue;
            }
            const KTextEditor::Cursor lineEnd(line, lineSize);
            KTextEditor::Range lineRange;
            if ( line == startLine ) {
                lineRange.setRange(range.start(), lineEnd);
            }
            else if ( line == endLine ) {
                lineRange.setRange(KTextEditor::Cursor(line, 0), range.end());
            }
            else {
                lineRange.setRange(KTextEditor::Cursor(line, 0), lineEnd);
            }
            // Off-load the actual work of highlighting the line to another call
            Q_ASSERT(lineRange.start().line() == lineRange.end().line());
            userChangedText(lineRange, user, removal);
        }
        return;
    }

    const QColor& userColor = ColorHelper::colorForUsername(user->name());
    foreach ( KTextEditor::MovingRange* existing, m_ranges ) {
        if ( existing->start() > range.end() || existing->end() < range.start() ) {
            continue;
        }

        bool colorMatches = existing->attribute()->background().color() == userColor;
        if ( colorMatches ) {
            if ( existing->contains(range) ) {
                // The existing range has the same color and contains the insertion.
                // It will auto-expand, and nothing needs to be done at all.
                return;
            }
            // Expand this range if it matches start or end of the new text
            if ( existing->start() == range.end() ) {
                existing->setRange(range.start(), existing->end());
                return;
            }
            else if ( existing->end() == range.start() ) {
                existing->setRange(existing->start(), range.end());
                return;
            }
            // Should not reach here, if I have understood the interface correctly :)
            Q_ASSERT(false);
        }
        else if ( existing->contains(range) ) {
            // split this range; the old range turns into the second part...
            KTextEditor::Cursor oldStart = existing->start();
            existing->setRange(range.end(), existing->end());
            // and a new one is created for the first part
            KTextEditor::Range firstPartRaw(oldStart, range.start());
            addHighlightedRange(firstPartRaw, existing->attribute()->background().color());
            // the range for the new text will be added below, after the loop.
        }
    }
    addHighlightedRange(range, userColor);
}

#include "documentchangetracker.moc"
