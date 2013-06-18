/* This file is part of the Kobby
 * Copyright (C) 2013 Sven Brauch <svenbrauch@gmail.com>
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

#include "kobbypluginview.h"
#include "manageddocument.h"
#include <common/ui/remotechangenotifier.h>
#include <libqinfinity/user.h>

KobbyPluginView::KobbyPluginView(KTextEditor::View* kteView)
    : QObject(kteView)
    , m_view(kteView)
{
}

void KobbyPluginView::remoteTextChanged(const KTextEditor::Range range, QInfinity::User* user)
{
    RemoteChangeNotifier::addNotificationWidget(m_view, range.start(), user->name());
}

void KobbyPluginView::documentReady(ManagedDocument* doc)
{
     connect(doc->textBuffer(), SIGNAL(remoteChangedText(KTextEditor::Range,QInfinity::User*)),
             this, SLOT(remoteTextChanged(KTextEditor::Range,QInfinity::User*)));
}

#include "kobbypluginview.moc"
