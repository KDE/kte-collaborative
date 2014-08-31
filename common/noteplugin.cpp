/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "noteplugin.h"
#include "document.h"

#include <KTextEditor/Document>
#include <KTextEditor/Editor>

#include <QDebug>

#include <libqinfinity/textsession.h>

#include <libinfinity/common/inf-user-table.h>
#include <libinftext/inf-text-session.h>
#include <libinftext/inf-text-default-buffer.h>

namespace Kobby
{

NotePlugin::NotePlugin( QObject *parent )
    : QInfinity::NotePlugin( "InfText", parent )
{
}

void NotePlugin::registerTextBuffer(const QString& /*path*/, KDocumentTextBuffer* /*textBuffer*/)
{
    // disabled until 0.7
//     Q_ASSERT( ! buffers.contains(path) );
//     buffers[path] = textBuffer;
}

QInfinity::Session *NotePlugin::createSession( QInfinity::CommunicationManager *commMgr,
    QInfinity::Session::Status sess_status,
    QInfinity::CommunicationGroup *syncGroup,
    QInfinity::XmlConnection *syncConnection,
    void* clientPluginUserData )
{
    QInfinity::TextBuffer* buffer = static_cast<QInfinity::TextBuffer*>(clientPluginUserData);
    QInfinity::TextSession *session = new QInfinity::TextSession( *commMgr,
        *buffer,
        sess_status,
        *syncGroup,
        *syncConnection );
    return session;
}

}

