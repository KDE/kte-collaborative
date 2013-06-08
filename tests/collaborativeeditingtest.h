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
#include <QProcess>
#include <QDir>
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
    inline KobbyPlugin* plugin_A() {
        return m_plugin_A;
    };
    inline KobbyPlugin* plugin_B() {
        return m_plugin_B;
    };
    inline KobbyPlugin* plugin(char whichPlugin) {
        return whichPlugin == 'A' ? plugin_A() : plugin_B();
    };
    KTextEditor::Document* newDocument(const QString& name, char whichPlugin);
    KTextEditor::Document* newDocument_A(const QString& name) {
        return newDocument(name, 'A');
    };
    KTextEditor::Document* newDocument_B(const QString& name) {
        return newDocument(name, 'B');
    };
    KTextEditor::Document* loadDocument(const QString& name, char whichPlugin);
    KTextEditor::Document* loadDocument_A(const QString& name) {
        return loadDocument(name, 'A');
    };
    KTextEditor::Document* loadDocument_B(const QString& name) {
        return loadDocument(name, 'B');
    };
    KTextEditor::Document* createDocumentInstance();
    QString userNameForPlugin(char whichPlugin) const;
    KUrl urlForFileName(const QString& fileName) const;
    void waitForDocument(KTextEditor::Document* document, KobbyPlugin* onPlugin);
    void waitForDocument_A(KTextEditor::Document* document) {
        return waitForDocument(document, plugin_A());
    }
    void waitForDocument_B(KTextEditor::Document* document) {
        return waitForDocument(document, plugin_B());
    }
    const QString serverDirectory() const {
        return QDir::tempPath() + "/kobby_unit_tests";
    };
    const int port() const {
        return 64261;
//         return 6523;
    };
    KobbyPlugin* m_plugin_A;
    KobbyPlugin* m_plugin_B;
    KService::Ptr m_documentService;
    QString makeFileName();
    QProcess* m_serverProcess;

};

#endif // COLLABORATIVEEDITINGTEST_H
