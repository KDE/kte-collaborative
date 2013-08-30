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

#ifndef KOBBY_NOTEPLUGIN_H
#define KOBBY_NOTEPLUGIN_H
#include "kobbycommon_export.h"

#include <libqinfinity/session.h>
#include <libqinfinity/noteplugin.h>
#include <KTextEditor/Editor>

#include "document.h"

namespace Kobby
{

class DocumentBuilder;

/**
 * @brief Instantiates InfText sessions.
 */
class KOBBYCOMMON_EXPORT NotePlugin
    : public QInfinity::NotePlugin
{
    public:
        NotePlugin( QObject *parent = 0 );

        void registerTextBuffer(const QString& path, KDocumentTextBuffer* textBuffer);

        QInfinity::Session *createSession(QInfinity::CommunicationManager *commMgr,
            QInfinity::Session::Status sess_status,
            QInfinity::CommunicationJoinedGroup *syncGroup,
            QInfinity::XmlConnection *syncConnection,
            const QString& path );

    private:
        QHash<QString, KDocumentTextBuffer*> buffers;

};

}

#endif

