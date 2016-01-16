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
#include "opencollabdocumentdialog.h"

// #include <ktpintegration/inftube.h>
// #include <KTp/Widgets/join-chat-room-dialog.h>
// #include <KTp/Widgets/contact-grid-dialog.h>
// #include <KTp/Models/contacts-filter-model.h>
#include <KLocalizedString>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KMessageWidget>
#include <KIO/Job>
#include <QPushButton>
#include <QFileDialog>

#include <QLayout>
#include <QCommandLinkButton>
#include <QGroupBox>
#include <QLabel>
// #include <TelepathyQt/PendingReady>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

// #include "ktpintegration/connectionswidget.h"

ShareDocumentDialog::ShareDocumentDialog(KTextEditor::View* activeView)
    : QDialog(activeView)
    , m_view(activeView)
    , m_tubeServer(0)
{
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QWidget* w = new QWidget();
    w->setLayout(new QVBoxLayout());
    mainLayout->addWidget(w);
    mainLayout->addWidget(buttonBox);

    KMessageWidget* infobox = new KMessageWidget;
    infobox->setCloseButtonVisible(false);
    infobox->setMessageType(KMessageWidget::Information);
    infobox->setWordWrap(true);
#if KDE_VERSION_MAJOR == 4 && KDE_VERSION_MINOR >= 11 || KDE_VERSION_MAJOR > 4
    infobox->setIcon(QIcon::fromTheme("help-about"));
#endif
    infobox->setText(i18n("<p>You can select a contact or chatroom to share this document with below.</p>"
                          "<p>Since each such connection to a contact or chatroom can contain "
                          "an arbitrary number of documents, you can also add this document "
                          "to an existing connection.</p>"));
    w->layout()->addWidget(infobox);

    QGroupBox* newConnectionBox = new QGroupBox();
    newConnectionBox->setTitle(i18n("Create a new connection"));
    newConnectionBox->setLayout(new QVBoxLayout());
    QCommandLinkButton* shareContactButton = new QCommandLinkButton(i18n("Share document with contact"));
    shareContactButton->setIcon(QIcon::fromTheme("im-user"));
    QCommandLinkButton* shareChatRoomButton = new QCommandLinkButton(i18n("Share document with chat room"));
    shareChatRoomButton->setIcon(QIcon::fromTheme("resource-group"));
    QCommandLinkButton* shareExistingServerButton = new QCommandLinkButton(i18n("Upload document to an existing server"));
    shareExistingServerButton->setIcon(QIcon::fromTheme("applications-internet"));
    newConnectionBox->layout()->addWidget(shareContactButton);
    newConnectionBox->layout()->addWidget(shareChatRoomButton);
    newConnectionBox->layout()->addWidget(shareExistingServerButton);
    w->layout()->addWidget(newConnectionBox);

    QGroupBox* addToExistingBox = new QGroupBox();
    addToExistingBox->setLayout(new QVBoxLayout());
    addToExistingBox->setTitle(i18n("Add document to existing connection"));
//     ConnectionsWidget* connections = new ConnectionsWidget();
//     connections->setHelpMessage(i18n("Click a row to add the document to that connection."));
//     addToExistingBox->layout()->addWidget(connections);
    w->layout()->addWidget(addToExistingBox);

    connect(shareContactButton, SIGNAL(clicked(bool)), SLOT(shareWithContact()));
    connect(shareChatRoomButton, SIGNAL(clicked(bool)), SLOT(shareWithChatRoom()));
    connect(shareExistingServerButton, SIGNAL(clicked(bool)), SLOT(putOnExistingServer()));
//     connect(connections, SIGNAL(connectionClicked(uint,QString)),
//             this, SLOT(shareWithExistingConnection(uint,QString)));

    resize(600, 450);

    connect(this, SIGNAL(shouldOpenDocument(KUrl)), this, SLOT(accept()));
}

const InfTubeRequester* ShareDocumentDialog::server() const
{
    return m_tubeServer;
}

void ShareDocumentDialog::jobFinished(KJob* job)
{
    qDebug() << "job finished";
    KIO::FileCopyJob* copyJob = qobject_cast<KIO::FileCopyJob*>(job);
    Q_ASSERT(job);
    emit shouldOpenDocument(copyJob->destUrl());
}

void ShareDocumentDialog::shareWithExistingConnection(uint port, QString nickname)
{
    qDebug() << "share with existing connection clicked";
    QUrl dest;
    dest.setAuthority("inf");
    dest.setHost("127.0.0.1");
    dest.setPort(port);
    dest.setUserName(nickname);
    dest.setPath(m_view->document()->url().fileName());
    KIO::FileCopyJob* job = KIO::file_copy(m_view->document()->url(), dest, -1, KIO::HideProgressInfo);
    connect(job, SIGNAL(finished(KJob*)), SLOT(jobFinished(KJob*)));
}

void ShareDocumentDialog::putOnExistingServer()
{
    QDialog serverParametersDialog;
#warning TODO
//     serverParametersDialog.button(QDialog::Ok)->setText(i18n("Connect"));
    HostSelectionWidget* w = new HostSelectionWidget;
    serverParametersDialog.resize(450, 200);
    if ( serverParametersDialog.exec() ) {
        foreach ( QWidget* w, findChildren<QWidget*>() ) {
            w->setDisabled(true);
        }
        QUrl result = QFileDialog::getSaveFileUrl(this, i18n("Save as"), w->selectedUrl());
        if ( result.isValid() ) {
            KIO::FileCopyJob* copyJob = KIO::file_copy(m_view->document()->url(), result);
            connect(copyJob, SIGNAL(finished(KJob*)), SLOT(jobFinished(KJob*)));
        }
        else {
            reject();
        }
    }
    else {
        reject();
    }
}

// void ShareDocumentDialog::shareWithContact()
// {
//     KTp::ContactGridDialog dialog(this);
//     dialog.filter()->setCapabilityFilterFlags(KTp::ContactsFilterModel::FilterByTubes);
//     dialog.filter()->setTubesFilterStrings(QStringList() << "infinote");
//     if ( dialog.exec() ) {
//         m_tubeServer = new InfTubeRequester(QApplication::instance());
//         KUrl url = m_view->document()->url();
//         if ( m_tubeServer->offer(dialog.account(), dialog.contact(), DocumentList() << url) ) {
//             m_view->document()->closeUrl();
//             m_tubeServer->setNicknameFromAccount(dialog.account());
//             connect(m_tubeServer, SIGNAL(collaborativeDocumentReady(KUrl)),
//                     this, SIGNAL(shouldOpenDocument(KUrl)));
//         }
//         else {
//             reject();
//         }
//     }
//     else {
//         reject();
//     }
// }
//
// void ShareDocumentDialog::shareWithChatRoom()
// {
//     m_tubeServer = new InfTubeRequester(QApplication::instance());
//     KTp::JoinChatRoomDialog dialog(m_tubeServer->connectionManager()->accountManager, this);
//     if ( dialog.exec() ) {
//         if ( m_tubeServer->offer(dialog.selectedAccount(), dialog.selectedChatRoom(), DocumentList() << m_view->document()->url()) ) {
//             m_tubeServer->setNicknameFromAccount(dialog.selectedAccount());
//             m_view->document()->closeUrl();
//             connect(m_tubeServer, SIGNAL(collaborativeDocumentReady(KUrl)),
//                     this, SIGNAL(shouldOpenDocument(KUrl)));
//         }
//         else {
//             accept();
//         }
//     }
//     else {
//         accept();
//     }
// }

#include "sharedocumentdialog.moc"
