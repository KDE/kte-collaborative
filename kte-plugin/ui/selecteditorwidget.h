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

#ifndef SELECTEDITORWIDGET_H
#define SELECTEDITORWIDGET_H

#include <QWidget>
#include <QMap>
#include <KDialog>

class QCheckBox;

/**
 * @brief Widget used in the KCM and the "Select editor" dialog to select the default application for collab documents.
 */
class SelectEditorWidget : public QWidget
{
Q_OBJECT
public:
    struct EditorEntry {
        QString readableName;
        QString command;
    };

    SelectEditorWidget(const QString& selectedEntry = QString(), QWidget* parent = 0, Qt::WindowFlags f = 0);

    /**
     * @brief Returns the currently selected entry
     * @return a struct describing the selected entry, i.e. its command and readable name.
     */
    EditorEntry selectedEntry() const;

signals:
    void selectionChanged();

private:
    QMap<QString, QString> m_validChoices;
    QWidget* m_buttonsGroup;
};

/**
 * @brief Simple dialog containing the above widget.
 */
class SelectEditorDialog : public KDialog
{
Q_OBJECT
public:
    SelectEditorDialog(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual void accept();

    /**
     * @brief @see \c SelectEditorWidget selectedEntry
     */
    SelectEditorWidget::EditorEntry selectedEntry() const;

private:
    SelectEditorWidget* m_selectWidget;
};

#endif // SELECTEDITORWIDGET_H
