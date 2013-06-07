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
#include "collaborativeeditingtest.h"
#include <kobby/kobbyplugin.h>
#include <ktexteditor/factory.h>

#include <QtTest>
#include <QtGlobal>
#include <QApplication>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KPluginFactory>
#include <KServiceTypeTrader>
#include <klibloader.h>

QTEST_MAIN(CollaborativeEditingTest);

void CollaborativeEditingTest::initTestCase()
{
    qDebug() << "initializing test case";
    KLibFactory* factory = KLibLoader::self()->factory("katepart");
    KTextEditor::Factory* kte_factory = qobject_cast<KTextEditor::Factory*>(factory);

    Q_ASSERT( kte_factory && "failed to instantiate kate part" );

    m_editor = kte_factory->editor();
    Q_ASSERT(m_editor);

    m_plugin = reinterpret_cast<KobbyPlugin*>(QApplication::instance()->property("KobbyPluginInstance").toLongLong());
    m_plugin->setProperty("useSimulatedConnection", true);
    m_plugin->setProperty("manageAllDocuments", true);
    qDebug() << "instance:" << m_plugin;
}

void CollaborativeEditingTest::cleanupTestCase()
{
    qDebug() << "cleaning up";
}

void CollaborativeEditingTest::init()
{

}

void CollaborativeEditingTest::cleanup()
{

}

void CollaborativeEditingTest::testTest()
{
    qDebug() << "running test test";
    KTextEditor::Document* doc1 = editor()->createDocument(this);
    KTextEditor::Document* doc2 = editor()->createDocument(this);
    QVERIFY(plugin()->managedDocuments().length() == 2);
}


#include "collaborativeeditingtest.moc"
