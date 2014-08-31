/*
 * Copyright 2013 Sven Brauch <svenbrauch@gmail.com>
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

#include "opencollabdocumentdialog.h"
#include <ktpintegration/connectionswidget.h>

#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

#include <KLocalizedString>
#include <KMessageWidget>
#include <KLineEdit>
#include <KFileDialog>
#include <KPushButton>

HostSelectionWidget::HostSelectionWidget(QWidget* parent)
    : QGroupBox(i18n("Manually enter connection parameters"), parent)
{
    // Construct the box for manually setting hostname etc.
    m_host = new KLineEdit();
    m_host->setClickMessage(i18nc("Examples for possible hostname formats", "e.g. 46.4.96.250, localhost, or mydomain.com"));
    m_port = new KLineEdit("6523");
    m_userName = new KLineEdit();
    m_userName->setClickMessage(i18n("The name you want to appear with in the session"));
    m_password = new KLineEdit();
    m_password->setClickMessage(i18n("Leave blank if not required by the server"));

    m_advancedSettingsLayout = new QFormLayout();
    setLayout(m_advancedSettingsLayout);
    m_tip = new KMessageWidget();
    m_tip->setMessageType(KMessageWidget::Information);
    m_tip->setWordWrap(true);
    m_tip->setText(i18n("If you want to save a set of manually entered parameters for later, just add "
                        "a bookmark in the file dialog which opens after entering the parameters. "
                        "When you want to re-use them, just select File -> Open and click the bookmark."));
    m_advancedSettingsLayout->addRow(m_tip);
    m_tip->setVisible(false); // Hide the tip, and only show it when needed
    m_advancedSettingsLayout->addRow(new QLabel(i18n("Remote host address:")), m_host);
    QPushButton* advancedButton = new QPushButton(i18n("Advanced..."));
    m_advancedSettingsLayout->addWidget(advancedButton);
    connect(advancedButton, SIGNAL(clicked(bool)), this, SLOT(showAdvanced(bool)));

    // Show the tip when the user actually uses the feature
    connect(advancedButton, SIGNAL(clicked(bool)), this, SLOT(showTip()));
    connect(m_host, SIGNAL(textChanged(QString)), this, SLOT(showTip()));

    m_host->setFocus();
}

KUrl HostSelectionWidget::selectedUrl() const
{
    KUrl url;
    url.setProtocol("inf");
    url.setPath("/");
    url.setHost(m_host->text());
    url.setPort(m_port->text().toInt());
    url.setUser(m_userName->text());
    url.setPassword(m_password->text());
    return url;
}

OpenCollabDocumentDialog::OpenCollabDocumentDialog(QWidget* parent, Qt::WindowFlags flags)
    : KDialog(parent, flags)
{
    QWidget* widget = new QWidget(this);
    widget->setLayout(new QVBoxLayout);

    m_manualSelectionWidget = new HostSelectionWidget(this);

    // Construct the box for selecting an existing connection
    QGroupBox* existingGroup = new QGroupBox(i18n("Choose an existing telepathy-based connection"));
    existingGroup->setLayout(new QHBoxLayout());
    ConnectionsWidget* connections = new ConnectionsWidget();
    connections->setHelpMessage(i18n("Click a connection to open a document from there."));
    existingGroup->layout()->addWidget(connections);

    // Put it all together
    widget->layout()->addWidget(m_manualSelectionWidget);
    widget->layout()->addWidget(existingGroup);

    connect(connections, SIGNAL(connectionClicked(uint,QString)),
            this, SLOT(connectionClicked(uint,QString)));

    connect(button(KDialog::Ok), SIGNAL(clicked(bool)), SLOT(acceptedWithManualConnection()));

    setMainWidget(widget);

    resize(600, 450);
}

void HostSelectionWidget::showTip()
{
    m_tip->setVisible(true);
}

void HostSelectionWidget::showAdvanced(bool)
{
    // Hide the "Advanced" button
    qobject_cast<QWidget*>(QObject::sender())->hide();
    m_advancedSettingsLayout->addRow(new QLabel(i18n("Port:")), m_port);
    m_advancedSettingsLayout->addRow(new QLabel(i18n("User name:")), m_userName);
    m_advancedSettingsLayout->addRow(new QLabel(i18n("Password (optional):")), m_password);
}

void OpenCollabDocumentDialog::connectionClicked(uint port, QString user)
{
    m_selectedConnection = qMakePair(port, user);
    accept();
    requestFileToOpen();
}

void OpenCollabDocumentDialog::acceptedWithManualConnection()
{
    requestFileToOpen();
}

void OpenCollabDocumentDialog::requestFileToOpen()
{
    // request URL to open
    KUrl result = KFileDialog::getOpenUrl(selectedBaseUrl());
    if ( result.isValid() ) {
        emit shouldOpenDocument(result);
        QDialog::accept();
    }
    else {
        reject();
    }
}

void OpenCollabDocumentDialog::accept()
{
    // Disable everything in this dialog; we'll close it later
    foreach ( QWidget* widget, findChildren<QWidget*>() ) {
        widget->setDisabled(true);
    }
}

KUrl OpenCollabDocumentDialog::selectedBaseUrl() const
{
    KUrl url;
    url.setProtocol("inf");
    url.setPath(QLatin1String("/"));
    if ( m_selectedConnection.first != 0 ) {
        // read parameters from clicked connection
        url.setHost("127.0.0.1");
        url.setPort(m_selectedConnection.first);
        url.setUser(m_selectedConnection.second);
    }
    else {
        // read parameters from manual selection
        url = m_manualSelectionWidget->selectedUrl();
    }
    return url;
}

#include "opencollabdocumentdialog.moc"
