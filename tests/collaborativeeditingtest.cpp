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
#include <QTcpServer>
#include <QTcpSocket>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KPluginFactory>
#include <KServiceTypeTrader>
#include <klibloader.h>

QTEST_MAIN(CollaborativeEditingTest);

void CollaborativeEditingTest::initTestCase()
{
    qDebug() << "initializing test case";

    // start the infinoted server needed for sharing documents
    QDir d;
    d.mkdir(serverDirectory());
    m_serverProcess = new QProcess;
    m_serverProcess->setEnvironment(QStringList() << "LIBINFINITY_DEBUG_PRINT_TRAFFIC=1");
    m_serverProcess->setStandardOutputFile(serverDirectory() + "/infinoted.log");
    m_serverProcess->setStandardErrorFile(serverDirectory() + "/infinoted.errors");
    m_serverProcess->start("/usr/bin/env", QStringList() << "infinoted-0.5" << "--security-policy=no-tls"
                                           << "-r" << serverDirectory() << "-p" << QString::number(port()));
    m_serverProcess->waitForStarted(500);
    // try binding to the server port; if this works, it has finished starting up
    while ( true ) {
        QTcpSocket s;
        s.connectToHost("localhost", port());
        if ( s.waitForConnected(20) ) {
            break;
        }
        QTest::qWait(50);
    }
    qDebug() << "successfully started infinioted";

    // get the service factory for documents and the plugin
    m_documentService = KService::serviceByDesktopPath("katepart.desktop");
    KService::Ptr pluginService = KService::serviceByDesktopPath("ktexteditor_kobby.desktop");

    // Instantiate a document. This loads an copy of the plugin, which we disable
    // (we want to use the instances we create below)
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
    QProcess kill;
    m_serverProcess->kill();
    // TODO this doesn't work, must clear the temporary files first
    QDir d;
    d.rmdir(serverDirectory());
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
    return whichPlugin == 'A' ? QString("USER_AAAAA") : QString("USER_BBBBB");
}

KUrl CollaborativeEditingTest::urlForFileName(const QString& fileName) const
{
    return KUrl("inf://localhost:" + QString::number(port()) + "/" + fileName);
}

void CollaborativeEditingTest::waitForDocument(KTextEditor::Document* document, KobbyPlugin* onPlugin)
{
    const ManagedDocumentList& docs = onPlugin->managedDocuments();
    bool ready = false;
    while ( ! ready ) {
        qDebug() << "waiting for document to become ready" << docs.isManaged(document) << document->url();
        QTest::qWait(1);
        QApplication::processEvents();
        if ( docs.isManaged(document) ) {
            ManagedDocument* managed = docs.findDocument(document);
            ready = managed->sessionStatus() == QInfinity::Session::Running;
            qDebug() << managed->sessionStatus() << managed->textBuffer();
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
    // For some reason, doc->saveAs() behaves in a weird way -- possibly due to the
    // intelligent caching stuff (it saves to a local document first, and later
    // uploads that to the server). I'm not sure what exactly causes it, but
    // using saveAs() instead of KIO::put causes the test to randomly fail.
    KobbyPlugin* p = plugin(whichPlugin);
    KUrl url = urlForFileName(name);
    url.setUser(userNameForPlugin(whichPlugin));
    KIO::put(url, 0);
    return loadDocument(name, whichPlugin);
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
    doc1->insertText(KTextEditor::Cursor(0, 0), "Hello World!");
    waitForDocument_A(doc1);
    waitForDocument_B(doc2);

    for ( int i = 0; i < 50; i++ ) {
        QTest::qWait(1);
        QApplication::processEvents();
    }

    qDebug() << doc1->text() << "|" << doc2->text();
    QVERIFY(doc1->text() == doc2->text());

    return;
}


#include "collaborativeeditingtest.moc"
