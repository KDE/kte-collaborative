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

#ifndef KCM_KTE_COLLABORATIVE_H
#define KCM_KTE_COLLABORATIVE_H

#include <KCModule>
#include <KConfigGroup>

class QCheckBox;
class QSlider;
class SelectEditorWidget;

/**
 * @brief This class is the configuration module for the plugin, accessible usually through the plugins list (wrench icon)
 */
class KCMKTECollaborative : public KCModule
{
public:
    explicit KCMKTECollaborative(QWidget* parent, const QVariantList& args);
    virtual ~KCMKTECollaborative();
    virtual void load();
    virtual void save();

private:
    // Group containing configuration related to colors.
    KConfigGroup m_colorsGroup;
    // Group containing configuration related to enabled notifier features,
    // especially background coloring + popup widgets
    KConfigGroup m_notifyGroup;
    // Group containing "default application" configuration
    KConfigGroup m_applicationsGroup;
    // colors
    QSlider* m_saturationSilder;
    // features
    QCheckBox* m_highlightBackground;
    QCheckBox* m_displayWidgets;
    QCheckBox* m_displayTextHints;
    // default application
    SelectEditorWidget* m_selectEditorWidget;
};

#endif // KCM_KTE_COLLABORATIVE_H
