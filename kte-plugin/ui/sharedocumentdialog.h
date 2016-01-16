/* This file is part of the Kobby plugin
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

#ifndef SHAREDOCUMENTDIALOG_H
#define SHAREDOCUMENTDIALOG_H

#include <QDialog>

class InfTubeRequester;
class ServerManager;
class KJob;
class HostSelectionWidget;

namespace KTextEditor {
    class View;
}

class ShareDocumentDialog : public QDialog
{
Q_OBJECT
public:
    explicit ShareDocumentDialog(KTextEditor::View* activeView);
    const InfTubeRequester* server() const;

private slots:
//     void shareWithContact();
//     void shareWithChatRoom();
    void shareWithExistingConnection(uint, QString);
    void jobFinished(KJob* job);
    void putOnExistingServer();

signals:
    void shouldOpenDocument(const QUrl& url);

private:
    KTextEditor::View* m_view;
    InfTubeRequester* m_tubeServer;
    HostSelectionWidget* m_hostSelectionWidget;
};

#endif // SHAREDOCUMENTDIALOG_H
