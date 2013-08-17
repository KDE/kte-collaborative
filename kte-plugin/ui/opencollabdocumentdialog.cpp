/*
 * <one line to give the library's name and an idea of what it does.>
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

#include "opencollabdocumentdialog.h"
#include <ktpintegration/connectionswidget.h>

#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>

#include <KLocalizedString>
#include <KMessageWidget>
#include <KLineEdit>

OpenCollabDocumentDialog::OpenCollabDocumentDialog(QWidget* parent, Qt::WindowFlags flags)
    : KDialog(parent, flags)
{
    QWidget* widget = new QWidget(this);
    widget->setLayout(new QVBoxLayout);

    // Construct the box for manually setting hostname etc.
    m_host = new KLineEdit();
    m_host->setClickMessage(i18nc("Examples for possible hostname formats", "e.g. 46.4.96.250, localhost, or mydomain.com"));
    m_port = new KLineEdit("6523");
    m_userName = new KLineEdit();
    m_userName->setClickMessage(i18n("The name you want to appear with in the session"));
    m_password = new KLineEdit();
    m_password->setClickMessage(i18n("Leave blank if not required by the server"));

    KMessageWidget* tip = new KMessageWidget();
    tip->setMessageType(KMessageWidget::Information);
    tip->setWordWrap(true);
    tip->setText(i18n("If you want to save a set of manually entered parameters for later, just add "
                      "a bookmark in the file dialog which opens after entering the parameters. "
                      "When you want to re-use them, just select File -> Open and click the bookmark."));
    widget->layout()->addWidget(tip);

    QGroupBox* manualGroup = new QGroupBox(i18n("Manually enter connection parameters"));
    QFormLayout* layout = new QFormLayout();
    manualGroup->setLayout(layout);
    layout->addRow(new QLabel(i18n("Remote host address:")), m_host);
    layout->addRow(new QLabel(i18n("Port:")), m_port);
    layout->addRow(new QLabel(i18n("User name:")), m_userName);
    layout->addRow(new QLabel(i18n("Password (optional):")), m_password);

    // Construct the box for selecting an existing connection
    QGroupBox* existingGroup = new QGroupBox(i18n("Choose an existing telepathy-based connection"));
    existingGroup->setLayout(new QHBoxLayout());
    ConnectionsWidget* connections = new ConnectionsWidget();
    existingGroup->layout()->addWidget(connections);

    // Put it all together
    widget->layout()->addWidget(manualGroup);
    widget->layout()->addWidget(existingGroup);

    connect(connections, SIGNAL(connectionClicked(uint,QString)),
            this, SLOT(connectionClicked(uint,QString)));

    setMainWidget(widget);

    resize(600, 450);
}

void OpenCollabDocumentDialog::connectionClicked(uint port, QString user)
{
    m_selectedConnection = qMakePair(port, user);
    accept();
}

KUrl OpenCollabDocumentDialog::selectedBaseUrl() const
{
    KUrl url;
    url.setProtocol("inf");
    url.setPath(QLatin1String("/"));
    if ( m_selectedConnection.first != 0 ) {
        // read parameters from clicked connection
        url.setHost("localhost");
        url.setPort(m_selectedConnection.first);
        url.setUser(m_selectedConnection.second);
    }
    else {
        // read parameters from manual selection
        url.setHost(m_host->text());
        url.setPort(m_port->text().toInt());
        url.setUser(m_userName->text());
        url.setPassword(m_password->text());
    }
    return url;
}

#include "opencollabdocumentdialog.moc"