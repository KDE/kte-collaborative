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

#ifndef OPENCOLLABDOCUMENTDIALOG_H
#define OPENCOLLABDOCUMENTDIALOG_H

#include <KDialog>

class KMessageWidget;
class QFormLayout;
class KLineEdit;

class OpenCollabDocumentDialog : public KDialog
{
Q_OBJECT
public:
    explicit OpenCollabDocumentDialog(QWidget* parent = 0, Qt::WindowFlags flags = 0);

    /**
     * @brief The URL the user has selected to open a document from. Only valid after the dialog was accepted.
     */
    KUrl selectedBaseUrl() const;
    virtual void accept();

signals:
    void shouldOpenDocument(const KUrl&);

public slots:
    void connectionClicked(uint,QString);
    void acceptedWithManualConnection();
    void showAdvanced(bool);

private slots:
    void showTip();
    void requestFileToOpen();

private:
    KLineEdit* m_password;
    KLineEdit* m_userName;
    KLineEdit* m_port;
    KLineEdit* m_host;
    QFormLayout* m_advancedSettingsLayout;
    KMessageWidget* m_tip;

    QPair<unsigned int, QString> m_selectedConnection;
};

#endif // OPENCOLLABDOCUMENTDIALOG_H
