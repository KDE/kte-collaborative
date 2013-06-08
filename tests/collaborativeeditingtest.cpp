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
#include <QMetaType>

#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KPluginFactory>
#include <KServiceTypeTrader>
#include <klibloader.h>

QTEST_MAIN(CollaborativeEditingTest);

using KTextEditor::Cursor;
using KTextEditor::Range;

void CollaborativeEditingTest::initTestCase()
{
    qDebug() << "initializing test case";

    // register types to use in data functions
    qRegisterMetaType< QList<Operation*> >("QList<Operation*>");

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
    QTime time = QTime::currentTime();
    qsrand( (uint) time.msec() + (uint) time.second()*60 + (uint) time.minute()*3600 + (uint) time.hour()*216000);
    return "kobby_test_" + QString::number(qrand());
}

void CollaborativeEditingTest::testInsertionConsistency()
{
    QString fileName = makeFileName();
    KTextEditor::Document* doc1 = newDocument_A(fileName);
    KTextEditor::Document* doc2 = loadDocument_B(fileName);
    KTextEditor::Document* raw = createDocumentInstance();

    QFETCH(QList<Operation*>, operations);
    replayTransaction(operations, doc1, doc2);
    replayTransaction(operations, raw);

    wait(NEED_SYNC_CYCLES);

    QCOMPARE(doc1->text(), doc2->text());
    QCOMPARE(doc1->text(), raw->text());

    delete doc1;
    delete doc2;
    delete raw;
    qDeleteAll(operations);
}

void CollaborativeEditingTest::testInsertionConsistency_data()
{
    QTest::addColumn< QList<Operation*> >("operations");

    QTest::newRow("base64") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "aAbBCcDdeEfF") );
    QTest::newRow("ascii") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "% A & ? A //  []") );
    QTest::newRow("newline") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "\n") );
    QTest::newRow("multiple_newlines") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "\n\n\n") );
    QTest::newRow("multiple_newlines_text") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "\naa\nc\n ") );
    QTest::newRow("unicode") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), QString::fromUtf8("\u20AC")) );

    QTest::newRow("multiple_operations") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "Foo")
                                                                 << new InsertOperation(Cursor(0, 0), "Bar")
                                                                 << new InsertOperation(Cursor(0, 0), "Baz") );
    QTest::newRow("multiple_operations_newlines") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "Foo\n")
                                                                 << new InsertOperation(Cursor(0, 0), "Bar\n\n")
                                                                 << new InsertOperation(Cursor(0, 0), "\n\nBaz") );
    QTest::newRow("multiple_operations_unicode") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), QString::fromUtf8("Fo\u20AC\n"))
                                                                 << new InsertOperation(Cursor(0, 0), QString::fromUtf8("B\u20ACr\n\n"))
                                                                 << new InsertOperation(Cursor(0, 0), QString::fromUtf8("\n\nBa\u20AC\u20AC")) );

    QTest::newRow("both_documents") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "Foo", 'A')
                                                                 << new InsertOperation(Cursor(0, 0), "Bar", 'B')
                                                                 << new WaitForSyncOperation('B')
                                                                 << new InsertOperation(Cursor(0, 3), "Baz", 'A') );

    QTest::newRow("both_documents_newlines") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "Foo\n", 'A')
                                                                 << new InsertOperation(Cursor(0, 0), "Bar", 'B')
                                                                 << new InsertOperation(Cursor(1, 0), "Baz", 'A') );
}

void CollaborativeEditingTest::testRemovalConsistency()
{
    QString fileName = makeFileName();
    KTextEditor::Document* doc1 = newDocument_A(fileName);
    KTextEditor::Document* doc2 = loadDocument_B(fileName);
    KTextEditor::Document* raw = createDocumentInstance();

    QFETCH(QList<Operation*>, operations);
    replayTransaction(operations, doc1, doc2);
    replayTransaction(operations, raw);

    wait(NEED_SYNC_CYCLES);

    QCOMPARE(doc1->text(), doc2->text());
    QCOMPARE(doc1->text(), raw->text());

    delete doc1;
    delete doc2;
    delete raw;
    qDeleteAll(operations);
}

void CollaborativeEditingTest::testRemovalConsistency_data()
{
    QTest::addColumn< QList<Operation*> >("operations");

    // Ctrl+A del
    QTest::newRow("delete_all") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "0123456789")
                                                        << new DeleteOperation(Range(Cursor(0, 0), Cursor(0, 10))) );
    QTest::newRow("delete_part") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "0123456789")
                                                        << new DeleteOperation(Range(Cursor(0, 0), Cursor(0, 3))) );
    // like holding del
    QTest::newRow("delete_multiple_begin") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "0123456789")
                                                        << new DeleteOperation(Range(Cursor(0, 0), Cursor(0, 1)))
                                                        << new DeleteOperation(Range(Cursor(0, 0), Cursor(0, 1)))
                                                        << new DeleteOperation(Range(Cursor(0, 0), Cursor(0, 1))) );
    // like holding backspace
    QTest::newRow("delete_multiple_end") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "0123456789")
                                                        << new DeleteOperation(Range(Cursor(0, 9), Cursor(0, 10)))
                                                        << new DeleteOperation(Range(Cursor(0, 8), Cursor(0, 9)))
                                                        << new DeleteOperation(Range(Cursor(0, 7), Cursor(0, 8))) );
    QTest::newRow("delete_newline") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "A\n")
                                                        << new DeleteOperation(Range(Cursor(0, 1), Cursor(0, 2))) );
    QTest::newRow("delete_newline_chars") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "AAA\nBBB")
                                                        << new DeleteOperation(Range(Cursor(0, 2), Cursor(1, 2))) );
    QTest::newRow("delete_unicode") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), QString::fromUtf8("\u20AC\u20ACA\n\u20AC\u20ACB"))
                                                        << new DeleteOperation(Range(Cursor(0, 1), Cursor(1, 1))) );
    QTest::newRow("delete_after_unicode") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), QString::fromUtf8("\u20ACabc"))
                                                        << new DeleteOperation(Range(Cursor(0, 2), Cursor(0, 3))) );

    QTest::newRow("delete_both_documents") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), QString::fromUtf8("0123456789"))
                                                        << new DeleteOperation(Range(Cursor(0, 2), Cursor(0, 3)), 'A')
                                                        << new WaitForSyncOperation('B')
                                                        << new DeleteOperation(Range(Cursor(0, 0), Cursor(0, 1)), 'B') );
}

void CollaborativeEditingTest::testBasicCorrectness()
{
    QString fileName = makeFileName();
    KTextEditor::Document* doc1 = newDocument_A(fileName);
    KTextEditor::Document* doc2 = loadDocument_B(fileName);
    KTextEditor::Document* raw = createDocumentInstance();

    QFETCH(QList<Operation*>, operations);
    QFETCH(QString, expectedText);
    replayTransaction(operations, doc1, doc2);
    replayTransaction(operations, raw);

    wait(NEED_SYNC_CYCLES);

    QCOMPARE(doc1->text(), doc2->text());
    QCOMPARE(doc1->text(), raw->text());
    QCOMPARE(doc1->text(), expectedText);

    delete doc1;
    delete doc2;
    delete raw;
    qDeleteAll(operations);
}

void CollaborativeEditingTest::testBasicCorrectness_data()
{
    QTest::addColumn< QList<Operation*> >("operations");
    QTest::addColumn< QString >("expectedText");

    QTest::newRow("basic_insert") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "0123456789") )
                                  << "0123456789";
    QTest::newRow("basic_delete") << ( QList<Operation*>() << new InsertOperation(Cursor(0, 0), "0123456789")
                                                           << new DeleteOperation(Range(Cursor(0, 0), Cursor(0, 3))) )
                                  << "3456789";
}


#include "collaborativeeditingtest.moc"
