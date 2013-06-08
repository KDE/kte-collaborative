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
#include <libqinfinity/xmppconnection.h>

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
//     KLibFactory* factory = KLibLoader::self()->factory("katepart");
//     KTextEditor::Factory* kte_factory = qobject_cast<KTextEditor::Factory*>(factory);

    m_documentService = KService::serviceByDesktopPath("katepart.desktop");
    KService::Ptr pluginService = KService::serviceByDesktopPath("ktexteditor_kobby.desktop");

    KTextEditor::Document* document = m_documentService->createInstance<KTextEditor::Document>(this);
    qDebug() << "got document from service:" << document;
    KobbyPlugin* p = reinterpret_cast<KobbyPlugin*>(QApplication::instance()->property("KobbyPluginInstance").toLongLong());
    qDebug() << "auto-created instance:" << p;
    p->setProperty("kobbyPluginDisabled", true);

    // TODO why do we use reinterpret_cast<> here? qobject_cast<> returns 0 for some reason,
    // although metaObject()->className() says "KobbyPlugin" and valgrind reports no errors
    // after accessing properties of the reinterpret_cast'ed object.
    m_plugin_A = reinterpret_cast<KobbyPlugin*>(pluginService->createInstance<KTextEditor::Plugin>(0));
    qDebug() << "got plugin A:" << plugin_A();

    m_plugin_B = reinterpret_cast<KobbyPlugin*>(pluginService->createInstance<KTextEditor::Plugin>(0));
    qDebug() << "got plugin B:" << plugin_B();
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

KTextEditor::Document* CollaborativeEditingTest::createDocumentInstance()
{
    return m_documentService->createInstance<KTextEditor::Document>(this);
}

QString CollaborativeEditingTest::userNameForPlugin(char whichPlugin) const
{
    return whichPlugin == 'A' ? QString("AAAAA") : QString("BBBBB");
}

KUrl CollaborativeEditingTest::urlForFileName(const QString& fileName) const
{
    return KUrl("inf://localhost/" + fileName);
}

void CollaborativeEditingTest::waitForDocument(KTextEditor::Document* document, KobbyPlugin* onPlugin)
{
    const ManagedDocumentList& docs = onPlugin->managedDocuments();
    bool ready = false;
    while ( ! ready ) {
        QApplication::processEvents();
        QTest::qWait(1);
        if ( docs.isManaged(document) ) {
            ManagedDocument* managed = docs.findDocument(document);
            ready = managed->sessionStatus() == QInfinity::Session::Running;
            ready = ready && managed->textBuffer()->hasUser();
        }
    }
}

KTextEditor::Document* CollaborativeEditingTest::loadDocument(const QString& name, char whichPlugin)
{
    KobbyPlugin* p = plugin(whichPlugin);
    KTextEditor::Document* doc = createDocumentInstance();
    p->addDocument(doc);
    KUrl url = urlForFileName(name);
    url.setUser(userNameForPlugin(whichPlugin));
    doc->openUrl(url);
    waitForDocument(doc, p);
    return doc;
}

KTextEditor::Document* CollaborativeEditingTest::newDocument(const QString& name, char whichPlugin)
{
    KobbyPlugin* p = plugin(whichPlugin);
    KTextEditor::Document* doc = createDocumentInstance();
    p->addDocument(doc);
    KUrl url = urlForFileName(name);
    url.setUser(userNameForPlugin(whichPlugin));
    doc->saveAs(url);
    waitForDocument(doc, p);
    return doc;
}

QString CollaborativeEditingTest::makeFileName()
{
    // TODO replace with a proper tempraryNote object
    // optimally a simple QTempFile with the proper url
    QTemporaryFile f;
    f.open();
    return KUrl(f.fileName()).fileName();
}

void CollaborativeEditingTest::testTest()
{
    QString fileName = makeFileName();
    KTextEditor::Document* doc1 = newDocument_A(fileName);
    KTextEditor::Document* doc2 = loadDocument_B(fileName);
    doc2->insertText(KTextEditor::Cursor(0, 0), "Hello World 2");
    waitForDocument_A(doc1);
    waitForDocument_B(doc2);

    qDebug() << doc1->text() << "|" << doc2->text();
    QVERIFY(doc1->text() == doc2->text());

    return; // TODO !!!!!!!!!!!!!!!!!!!!!!!
//     qDebug() << "running test test";
//     KTextEditor::Document* doc1 = editor()->createDocument(this);
//     doc1->saveAs(KUrl("inf://localhost/unittest.txt"));
//     qDebug() << "new document url:" << doc1->url();
//     for ( int i = 0; i < 200; i++ ) {
//         QTest::qWait(1);
//         QApplication::processEvents();
//     }
//     KTextEditor::Document* doc2 = editor()->createDocument(this);
//     doc2->openUrl(KUrl("inf://localhost/unittest.txt"));
//     for ( int i = 0; i < 200; i++ ) {
//         QTest::qWait(1);
//         QApplication::processEvents();
//     }
//     QVERIFY(plugin()->managedDocuments().length() == 2);
//     doc1->insertText(KTextEditor::Cursor(0, 0), "Hello World");
//     QApplication::processEvents();
//     QVERIFY(doc1->text() == "Hello World");
//     QVERIFY(doc2->text() == "Hello World");
}


#include "collaborativeeditingtest.moc"
