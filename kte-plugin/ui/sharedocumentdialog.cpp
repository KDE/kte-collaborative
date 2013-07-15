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

#include "sharedocumentdialog.h"

#include <ktpintegration/inftube.h>
#include <KTp/Widgets/join-chat-room-dialog.h>
#include <KTp/Widgets/contact-grid-dialog.h>
#include <KLocalizedString>
#include <KTextEditor/View>
#include <KTextEditor/Document>

#include <QLayout>
#include <QCommandLinkButton>
#include <TelepathyQt/PendingReady>

ShareDocumentDialog::ShareDocumentDialog(KTextEditor::View* activeView)
    : KDialog(activeView)
    , m_view(activeView)
{
    setButtons(Cancel);
    QWidget* w = new QWidget();
    w->setLayout(new QVBoxLayout());
    setMainWidget(w);
    QCommandLinkButton* shareContactButton = new QCommandLinkButton(i18n("Share document with contact"));
    QCommandLinkButton* shareChatRoomButton = new QCommandLinkButton(i18n("Share document with chat room"));
    w->layout()->addWidget(shareContactButton);
    w->layout()->addWidget(shareChatRoomButton);
    connect(shareContactButton, SIGNAL(clicked(bool)), SLOT(shareWithContact()));
    connect(shareChatRoomButton, SIGNAL(clicked(bool)), SLOT(shareWithChatRoom()));
}

const InfTubeServer* ShareDocumentDialog::server() const
{
    return m_tubeServer;
}

void ShareDocumentDialog::shareWithContact()
{
    accept();
    KTp::ContactGridDialog dialog(this);
    if ( dialog.exec() ) {
        m_tubeServer = new InfTubeServer(this);
        KUrl url = m_view->document()->url();
        if ( m_tubeServer->offer(dialog.account(), dialog.contact(), DocumentList() << url) ) {
            m_view->document()->closeUrl();
            m_tubeServer->setNicknameFromAccount(dialog.account());
        }
    }
}

void ShareDocumentDialog::shareWithChatRoom()
{
    accept();
    m_tubeServer = new InfTubeServer(this);
    KTp::JoinChatRoomDialog dialog(m_tubeServer->connectionManager()->accountManager, this);
    if ( dialog.exec() ) {
        if ( m_tubeServer->offer(dialog.selectedAccount(), dialog.selectedChatRoom(), DocumentList() << m_view->document()->url()) ) {
            m_tubeServer->setNicknameFromAccount(dialog.selectedAccount());
            m_view->document()->closeUrl();
            connect(m_tubeServer, SIGNAL(fileCopiedToServer(KUrl)), this, SIGNAL(shouldOpenDocument(KUrl)));
        }
    }
}


