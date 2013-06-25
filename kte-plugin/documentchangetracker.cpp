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

void DocumentChangeTracker::userChangedText(const KTextEditor::Range& range, QInfinity::User* user, bool removal)
{
    kDebug() << "user changed text:" << user->name() << range << removal;
    if ( ! iface() ) {
        return;
    }
    if ( ! removal ) {
        KTextEditor::MovingRange* r;
        r = iface()->newMovingRange(range, KTextEditor::MovingRange::DoNotExpand, KTextEditor::MovingRange::InvalidateIfEmpty);
        KTextEditor::Attribute::Ptr attrib(new KTextEditor::Attribute);
        attrib->setBackground(user->color());
        r->setAttribute(attrib);
    }
}

#include "documentchangetracker.moc"
