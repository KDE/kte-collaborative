/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  <copyright holder> <email>
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

#ifndef COLLABORATIVEEDITINGTEST_H
#define COLLABORATIVEEDITINGTEST_H

#include <QObject>
#include <KTextEditor/Editor>
#include <kobbyplugin.h>

class CollaborativeEditingTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testTest();

private:
    inline KTextEditor::Editor* editor() const { return m_editor; }
    inline KobbyPlugin* plugin() const { return m_plugin; }
    KTextEditor::Editor* m_editor;
    KobbyPlugin* m_plugin;

};

#endif // COLLABORATIVEEDITINGTEST_H
