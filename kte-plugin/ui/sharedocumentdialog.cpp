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
#include <KTp/Models/contacts-filter-model.h>
#include <KLocalizedString>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KMessageWidget>
#include <KIO/Job>

#include <QLayout>
#include <QCommandLinkButton>
#include <QGroupBox>
#include <QLabel>
#include <TelepathyQt/PendingReady>

#include "ktpintegration/connectionswidget.h"

ShareDocumentDialog::ShareDocumentDialog(KTextEditor::View* activeView)
    : KDialog(activeView)
    , m_view(activeView)
    , m_tubeServer(0)
{
    setButtons(Cancel);
    QWidget* w = new QWidget();
    w->setLayout(new QVBoxLayout());
    setMainWidget(w);

    KMessageWidget* infobox = new KMessageWidget;
    infobox->setCloseButtonVisible(false);
    infobox->setMessageType(KMessageWidget::Information);
    infobox->setWordWrap(true);
    infobox->setIcon(KIcon("help-about"));
    infobox->setText(i18n("<p>You can select a contact or chatroom to share this document with below.</p>"
                          "<p>Since each such connection to a contact or chatroom can contain "
                          "an arbitrary number of documents, you can also add this document "
                          "to an existing connection.</p>"));
    w->layout()->addWidget(infobox);

    QGroupBox* newConnectionBox = new QGroupBox();
    newConnectionBox->setTitle(i18n("Create a new connection"));
    newConnectionBox->setLayout(new QVBoxLayout());
    QCommandLinkButton* shareContactButton = new QCommandLinkButton(i18n("Share document with contact"));
    QCommandLinkButton* shareChatRoomButton = new QCommandLinkButton(i18n("Share document with chat room"));
    newConnectionBox->layout()->addWidget(shareContactButton);
    newConnectionBox->layout()->addWidget(shareChatRoomButton);
    w->layout()->addWidget(newConnectionBox);

    QGroupBox* addToExistingBox = new QGroupBox();
    addToExistingBox->setLayout(new QVBoxLayout());
    addToExistingBox->setTitle(i18n("Add document to existing connection"));
    ConnectionsWidget* connections = new ConnectionsWidget();
    connections->setHelpMessage(i18n("Click a row to add the document to that connection."));
    addToExistingBox->layout()->addWidget(connections);
    w->layout()->addWidget(addToExistingBox);

    connect(shareContactButton, SIGNAL(clicked(bool)), SLOT(shareWithContact()));
    connect(shareChatRoomButton, SIGNAL(clicked(bool)), SLOT(shareWithChatRoom()));
    connect(connections, SIGNAL(connectionClicked(uint,QString)),
            this, SLOT(shareWithExistingConnection(uint,QString)));

    resize(600, 450);

    connect(this, SIGNAL(shouldOpenDocument(KUrl)), this, SLOT(accept()));
}

const InfTubeRequester* ShareDocumentDialog::server() const
{
    return m_tubeServer;
}

void ShareDocumentDialog::jobFinished(KJob* job)
{
    kDebug() << "job finished";
    KIO::FileCopyJob* copyJob = qobject_cast<KIO::FileCopyJob*>(job);
    Q_ASSERT(job);
    emit shouldOpenDocument(copyJob->destUrl());
}

void ShareDocumentDialog::shareWithExistingConnection(uint port, QString nickname)
{
    kDebug() << "share with existing connection clicked";
    KUrl dest;
    dest.setProtocol("inf");
    dest.setHost("127.0.0.1");
    dest.setPort(port);
    dest.setUser(nickname);
    dest.setFileName(m_view->document()->url().fileName());
    KIO::FileCopyJob* job = KIO::file_copy(m_view->document()->url(), dest, -1, KIO::HideProgressInfo);
    connect(job, SIGNAL(finished(KJob*)), SLOT(jobFinished(KJob*)));
}

void ShareDocumentDialog::shareWithContact()
{
    KTp::ContactGridDialog dialog(this);
    dialog.filter()->setCapabilityFilterFlags(KTp::ContactsFilterModel::FilterByTubes);
    dialog.filter()->setTubesFilterStrings(QStringList() << "infinote");
    if ( dialog.exec() ) {
        m_tubeServer = new InfTubeRequester(QApplication::instance());
        KUrl url = m_view->document()->url();
        if ( m_tubeServer->offer(dialog.account(), dialog.contact(), DocumentList() << url) ) {
            m_view->document()->closeUrl();
            m_tubeServer->setNicknameFromAccount(dialog.account());
            connect(m_tubeServer, SIGNAL(collaborativeDocumentReady(KUrl)),
                    this, SIGNAL(shouldOpenDocument(KUrl)));
        }
        else {
            accept();
        }
    }
    else {
        accept();
    }
}

void ShareDocumentDialog::shareWithChatRoom()
{
    m_tubeServer = new InfTubeRequester(QApplication::instance());
    KTp::JoinChatRoomDialog dialog(m_tubeServer->connectionManager()->accountManager, this);
    if ( dialog.exec() ) {
        if ( m_tubeServer->offer(dialog.selectedAccount(), dialog.selectedChatRoom(), DocumentList() << m_view->document()->url()) ) {
            m_tubeServer->setNicknameFromAccount(dialog.selectedAccount());
            m_view->document()->closeUrl();
            connect(m_tubeServer, SIGNAL(collaborativeDocumentReady(KUrl)),
                    this, SIGNAL(shouldOpenDocument(KUrl)));
        }
        else {
            accept();
        }
    }
    else {
        accept();
    }
}

#include "sharedocumentdialog.moc"
