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

#include "selecteditorwidget.h"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QCheckBox>
#include <QVariant>

#include <KLocalizedString>
#include <KDebug>
#include <kconfig.h>

SelectEditorWidget::SelectEditorWidget(const QString& selectedEntry, QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    // TODO: grey those out which are not installed
    // %u: full URL; %d: full URL without filename; %h: hostname:port
    m_validChoices.insert("dolphin %d", i18n("Display folder in Dolphin"));
    m_validChoices.insert("gobby-0.5 -c %h", i18n("Start %1", QLatin1String("Gobby")));
    m_validChoices.insert("kwrite %u", i18n("Open in %1", QLatin1String("KWrite")));
    m_validChoices.insert("kate %u", i18n("Open in %1", QLatin1String("Kate")));
    m_validChoices.insert("kile %u", i18n("Open in %1", QLatin1String("Kile")));
    // TODO implement this in KDevelop first
//     m_validChoices.insert("kdevelop -f %u", i18n("Open in %1", "KDevelop"));

    m_buttonsGroup = new QWidget;
    m_buttonsGroup->setLayout(new QVBoxLayout);
    foreach ( const QString& choice, m_validChoices.keys() ) {
        const QString& readableName = m_validChoices[choice];
        kDebug() << choice << m_validChoices[choice] << readableName;
        QRadioButton* radio = new QRadioButton(readableName);
        radio->setProperty("command", choice);
        if ( selectedEntry == choice ) {
            // pre-select the passed one
            radio->setChecked(true);
        }
        connect(radio, SIGNAL(toggled(bool)), this, SIGNAL(selectionChanged()));
        m_buttonsGroup->layout()->addWidget(radio);
    }

    setLayout(new QVBoxLayout);
    layout()->addWidget(new QLabel(i18n("What action should be taken for collaborative documents by default?")));
    layout()->addWidget(m_buttonsGroup);
}

QPair< QString, QString > SelectEditorWidget::selectedEntry() const
{
    foreach ( const QRadioButton* button, m_buttonsGroup->findChildren<QRadioButton*>() ) {
        if ( button->isChecked() ) {
            const QString& command = button->property("command").toString();
            return QPair<QString, QString>(command, m_validChoices[command]);
        }
    }
    return QPair<QString, QString>();
}

SelectEditorDialog::SelectEditorDialog(QWidget* parent, Qt::WindowFlags flags)
    : KDialog(parent, flags)
{
    QWidget* main = new QWidget();
    main->setLayout(new QVBoxLayout);
    setMainWidget(main);
    QLabel* text = new QLabel(i18n("It seems you are trying to open a collaborative document for the first time."));
    text->setWordWrap(true);
    main->layout()->addWidget(text);

    m_selectWidget = new SelectEditorWidget();
    main->layout()->addWidget(m_selectWidget);
}

QPair< QString, QString > SelectEditorDialog::selectedEntry() const
{
    return m_selectWidget->selectedEntry();
}

void SelectEditorDialog::accept()
{
    KConfig config("ktecollaborative");
    KConfigGroup group(config.group("applications"));
    group.writeEntry("editor", m_selectWidget->selectedEntry().first);
    KDialog::accept();
}

#include "selecteditorwidget.moc"
