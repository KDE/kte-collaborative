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

#include <libqinfinity/session.h>
#include <libqinfinity/noteplugin.h>
#include "document.h"
#include <KTextEditor/Editor>

namespace Kobby
{

class DocumentBuilder;

/**
 * @brief Instantiates InfText sessions.
 */
class NotePlugin
    : public QInfinity::NotePlugin
{

    public:
        NotePlugin( KTextEditor::Editor* editor, KDocumentTextBuffer* buffer,
            QObject *parent = 0 );

        QInfinity::Session *createSession(QInfinity::CommunicationManager *commMgr,
            QInfinity::Session::Status sess_status,
            QInfinity::CommunicationJoinedGroup *syncGroup,
            QInfinity::XmlConnection *syncConnection );

    private:
        KTextEditor::Editor* m_editor;
        KDocumentTextBuffer* m_buffer;

};

}

#endif

