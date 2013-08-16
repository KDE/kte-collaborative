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

#include "kobbypluginview.h"
#include "manageddocument.h"
#include "kobbyplugin.h"
#include "ui/remotechangenotifier.h"
#include "ui/sharedocumentdialog.h"
#include "documentchangetracker.h"
#include "settings/kcm_kte_collaborative.h"
#include "ktpintegration/inftube.h"
#include <common/utils.h>

#include <libqinfinity/user.h>
#include <libqinfinity/usertable.h>
#include <KTp/Widgets/contact-grid-dialog.h>
#include <KTp/Widgets/join-chat-room-dialog.h>
#include <ktexteditor/texthintinterface.h>

#include <QLayout>
#include <QLabel>
#include <QAction>
#include <QLineEdit>
#include <QFormLayout>
#include <QCommandLinkButton>

#include <KLocalizedString>
#include <KActionCollection>
#include <KIcon>
#include <KAction>
#include <KDialog>
#include <KMessageBox>
#include <KColorScheme>
#include <KTextEditor/Editor>
#include <KFileDialog>
#include <KCMultiDialog>
#include <KRun>

void setTextColor(QWidget* textWidget, KColorScheme::ForegroundRole colorRole) {
    QPalette p = textWidget->palette();
    KColorScheme scheme(QPalette::Active);
    QBrush brush = scheme.foreground(colorRole);
    p.setBrush(QPalette::Foreground, brush);
    textWidget->setPalette(p);
}

KobbyStatusBar::KobbyStatusBar(KobbyPluginView* parent, Qt::WindowFlags f)
    : QWidget(parent->m_view, f)
    , m_connectionStatusLabel(new QLabel(this))
    , m_usersLabel(new QLabel(this))
    , m_view(parent)
{
    setLayout(new QHBoxLayout());
    layout()->setAlignment(Qt::AlignRight);
    layout()->addWidget(m_usersLabel);
    layout()->addWidget(m_connectionStatusLabel);
}

void KobbyStatusBar::connectionStatusChanged(Kobby::Connection*, QInfinity::XmlConnection::Status status)
{
    QString text;
    KColorScheme::ForegroundRole role = KColorScheme::NormalText;
    if ( status == QInfinity::XmlConnection::Closed ) {
        // Since the connection will start opening immediately,
        // this will not display in the beginning, just on disconnect
        text = "<b>" + i18n("Disconnected from collaboration server.") + "</b>";
        role = KColorScheme::NegativeText;
        m_usersLabel->setText(QString());
    }
    else if ( status == QInfinity::XmlConnection::Opening ) {
        text = i18n("Connecting...");
    }
    else if ( status == QInfinity::XmlConnection::Open ) {
        text = i18n("Synchronizing document...");
    }
    else if ( status == QInfinity::XmlConnection::Closing ) {
        text = i18n("Disconnecting...");
    }
    setTextColor(m_connectionStatusLabel, role);
    m_connectionStatusLabel->setText(text);
}

void KobbyStatusBar::sessionFullyReady()
{
    setTextColor(m_connectionStatusLabel, KColorScheme::PositiveText);
    m_connectionStatusLabel->setText( "<b>" + i18n("Connected to collaboration server.") + "</b>" );
}

void KobbyStatusBar::usersChanged()
{
    if ( ! m_view->m_document->userTable() ) {
        return;
    }
    QList< QPointer< QInfinity::User > > users = m_view->m_document->userTable()->users();
    foreach ( const QPointer<QInfinity::User>& user, users ) {
        connect(user.data(), SIGNAL(statusChanged()), this, SLOT(usersChanged()), Qt::UniqueConnection);
    }
    QList< QPointer< QInfinity::User > > activeUsers = m_view->m_document->userTable()->activeUsers();
    if ( activeUsers.length() > 4 ) {
        m_usersLabel->setText(i18n("Users: You and %1 others", activeUsers.size() - 1));
    }
    else {
        QStringList usersList;
        foreach ( const QPointer<QInfinity::User>& user, activeUsers ) {
            if ( user->name() != m_view->m_document->textBuffer()->user()->name() ) {
                usersList.append(user->name());
            }
        }
        if ( usersList.length() == 1 ) {
            m_usersLabel->setText(i18nc("As in Users: You and Fred", "Users: You and %1", usersList.first()));
        }
        else if ( usersList.length() > 1 ) {
            m_usersLabel->setText(i18nc("As in Users: You, Fred, George", "Users: You, %1", usersList.join(", ")));
        }
        else {
            m_usersLabel->setText(i18n("Users: only you"));
        }
    }
}

KobbyStatusBar* KobbyPluginView::statusBar() const
{
    return m_statusBar;
}

KobbyPluginView::KobbyPluginView(KTextEditor::View* kteView, ManagedDocument* document)
    : QObject(kteView)
    , KXMLGUIClient(kteView)
    , m_view(kteView)
    , m_statusBar(0)
    , m_document(document)
{
    setComponentData(KobbyPluginFactory::componentData());
    setXMLFile("ktexteditor_kobbyui.rc");

    // Set up the actions for the "Collaborative" menu
    m_openCollabDocumentAction = actionCollection()->addAction("kobby_open", this, SLOT(openActionClicked()));
    m_openCollabDocumentAction->setText(i18n("Open collaborative document..."));
    m_openCollabDocumentAction->setToolTip(i18n("Open a collaborative document from a manually specified server"));
    m_openCollabDocumentAction->setShortcut(KShortcut(QKeySequence("Ctrl+Meta+O")), KAction::DefaultShortcut);
    m_openCollabDocumentAction->setIcon(KIcon("document-open"));

    m_saveCopyAction = actionCollection()->addAction("kobby_save_copy", this, SLOT(saveCopyActionClicked()));
    m_saveCopyAction->setText(i18n("Save a local copy..."));
    m_saveCopyAction->setHelpText(i18n("Save a local copy of the current document, but continue "
                                       "synchronizing changes"));
    m_saveCopyAction->setShortcut(KShortcut(QKeySequence("Ctrl+Meta+S")), KAction::DefaultShortcut);
    m_saveCopyAction->setIcon(KIcon("document-save-as"));

    m_openFileManagerAction = actionCollection()->addAction("kobby_open_file_manager", this, SLOT(openFileManagerActionClicked()));
    m_openFileManagerAction->setText(i18n("Open file manager"));
    m_openFileManagerAction->setShortcut(KShortcut(QKeySequence("Ctrl+Meta+F")), KAction::DefaultShortcut);
    m_openFileManagerAction->setIcon(KIcon("system-file-manager"));

    m_shareWithContactAction = actionCollection()->addAction("kobby_share_with_contact", this, SLOT(shareActionClicked()));
    m_shareWithContactAction->setText(i18n("Share document..."));
    m_shareWithContactAction->setHelpText(i18n("Collaboratively edit the current document with an "
                                               "instant messenger contact"));
    m_shareWithContactAction->setShortcut(KShortcut(QKeySequence("Ctrl+Meta+E")), KAction::DefaultShortcut);
    m_shareWithContactAction->setIcon(KIcon("document-share"));
    m_shareWithContactAction->setEnabled(m_view->document()->url().protocol() != "inf");

    m_changeUserNameAction = actionCollection()->addAction("kobby_change_user_name", this, SLOT(changeUserActionClicked()));
    m_changeUserNameAction->setText(i18n("Change user name..."));
    m_changeUserNameAction->setHelpText(i18n("Change your user name for the current document"));
    m_changeUserNameAction->setShortcut(KShortcut(QKeySequence("Ctrl+Meta+U")), KAction::DefaultShortcut);
    m_changeUserNameAction->setIcon(KIcon("system-users"));

    m_disconnectAction = actionCollection()->addAction("kobby_disconnect", this, SLOT(disconnectActionClicked()));
    m_disconnectAction->setText(i18n("Disconnect"));
    m_disconnectAction->setHelpText(i18n("Disconnect from the collaborative server, and stop "
                                         "synchronizing changes to the document"));
    m_disconnectAction->setShortcut(KShortcut(QKeySequence("Ctrl+Meta+W")), KAction::DefaultShortcut);
    m_disconnectAction->setIcon(KIcon("network-disconnect"));

    m_createServerAction = actionCollection()->addAction("kobby_create_server", this, SLOT(createServerActionClicked()));
    m_createServerAction->setText(i18n("Start collaborative server..."));
    m_createServerAction->setHelpText(i18n("Host a collaboration session. Other persons can join the session "
                                           "by using the \"Open collaborative document\" action."));
    m_createServerAction->setShortcut(KShortcut(QKeySequence("Ctrl+Meta+N")), KAction::DefaultShortcut);
    m_createServerAction->setIcon(KIcon("network-connect"));

    // TODO: disable this for non-collab documents
    m_clearHighlightAction = actionCollection()->addAction("kobby_clear_highlight", this, SLOT(clearHighlightActionClicked()));
    m_clearHighlightAction->setText(i18n("Clear user highlights"));
    m_clearHighlightAction->setShortcut(KShortcut(QKeySequence("Ctrl+Meta+C")), KAction::DefaultShortcut);
    m_clearHighlightAction->setIcon(KIcon("edit-clear"));

    m_configureAction = actionCollection()->addAction("kobby_configure", this, SLOT(configureActionClicked()));
    m_configureAction->setText(i18n("Configure..."));
    m_configureAction->setIcon(KIcon("configure"));

    m_actionsRequiringConnection << m_saveCopyAction << m_changeUserNameAction
                                 << m_disconnectAction << m_clearHighlightAction
                                 << m_openFileManagerAction;

    disableActions();
    if ( m_document ) {
        // See KobbyPlugin::addView
        documentBecameManaged(document);
    }
}

void KobbyPluginView::openFileManagerActionClicked()
{
    if ( ! m_document || ! m_document->document()->url().isValid() ) {
        return;
    }
    KUrl url = m_document->document()->url();
    if ( url.protocol() != "inf" ) {
        return;
    }
    // TODO: make sure to use a KDE file manager, since others don't use KIO?
    KRun::runUrl(url.upUrl(), KMimeType::findByUrl(url.upUrl())->name(), m_view);
}

void KobbyPluginView::configureActionClicked()
{
    KCMultiDialog proxy;
    proxy.addModule("ktexteditor_kobby_config");
    proxy.exec();
}

void KobbyPluginView::clearHighlightActionClicked()
{
    if ( m_document && m_document->changeTracker() ) {
        m_document->changeTracker()->clearHighlight();
    }
}

void KobbyPluginView::disableActions()
{
    foreach ( KAction* action, m_actionsRequiringConnection ) {
        action->setEnabled(false);
    }
    m_view->action("file_save")->setEnabled(true);
}

void KobbyPluginView::enableActions()
{
    foreach ( KAction* action, m_actionsRequiringConnection ) {
        action->setEnabled(true);
    }
    // When enabling the collaborative actions,
    // we want to disable the built-in save and undo.
    // TODO make Ctrl+S call the "Save copy" action somehow?
    m_view->action("file_save")->setEnabled(false);
}

void KobbyPluginView::documentBecameManaged(ManagedDocument* document)
{
    if ( document->document() != m_view->document() ) {
        return;
    }
    m_document = document;
    enableUi();
}

void KobbyPluginView::documentBecameUnmanaged(ManagedDocument* document)
{
    if ( document != m_document || m_document == 0 ) {
        return;
    }
    Q_ASSERT(document->document() == m_document->document());
    m_document = 0;
    disableUi();
    disableActions();
}

void KobbyPluginView::textHintRequested(const KTextEditor::Cursor& position, QString& hint)
{
    hint = i18nc("%1 is a user name", "Written by: <b>%1</b>", m_document->changeTracker()->userForCursor(position));
}

void KobbyPluginView::enableUi()
{
    m_statusBar = new KobbyStatusBar(this);
    connect(m_document->connection(), SIGNAL(statusChanged(Connection*,QInfinity::XmlConnection::Status)),
            statusBar(), SLOT(connectionStatusChanged(Connection*,QInfinity::XmlConnection::Status)), Qt::UniqueConnection);
    statusBar()->connectionStatusChanged(m_document->connection(), m_document->connection()->status());
    connect(m_document, SIGNAL(documentReady(ManagedDocument*)),
            this, SLOT(documentReady(ManagedDocument*)), Qt::UniqueConnection);
    m_view->layout()->addWidget(m_statusBar);
    m_shareWithContactAction->setEnabled(false);

    KConfig config("ktecollaborative");
    KConfigGroup group = config.group("notifications");
    if ( group.readEntry("enableTextHints", true) ) {
        if ( KTextEditor::TextHintInterface* iface = qobject_cast<KTextEditor::TextHintInterface*>(m_view) ) {
            iface->enableTextHints(300);
            connect(m_view, SIGNAL(needTextHint(const KTextEditor::Cursor&,QString&)),
                    this, SLOT(textHintRequested(KTextEditor::Cursor,QString&)));
        }
    }
}

void KobbyPluginView::disableUi()
{
    m_view->layout()->removeWidget(m_statusBar);
    delete m_statusBar;
    m_statusBar = 0;
    m_shareWithContactAction->setEnabled(true);
    if ( KTextEditor::TextHintInterface* iface = qobject_cast<KTextEditor::TextHintInterface*>(m_view) ) {
        iface->disableTextHints();
    }
    // Connections are disconnected automatically since m_document will be deleted
}

void KobbyPluginView::disconnectActionClicked()
{
    // TODO
    m_document->document()->saveAs(KUrl(QDir::tempPath() + m_document->document()->url().encodedPath()));
}

void KobbyPluginView::changeUserActionClicked()
{
    if ( ! m_document || ! m_document->textBuffer() || ! m_document->textBuffer()->user() ) {
        KMessageBox::error(m_view, i18n("You cannot change your user name for a document you are not subscribed to."));
        return;
    }
    KDialog dialog(m_view);
    dialog.setCaption(i18n("Change user name"));
    dialog.setButtons(KDialog::Ok | KDialog::Cancel);
    dialog.setDefaultButton(KDialog::Ok);
    QWidget* widget = new QWidget(&dialog);
    widget->setLayout(new QVBoxLayout);
    widget->layout()->addWidget(new QLabel(i18n("Enter your new user name:")));
    QLineEdit* lineEdit = new QLineEdit();
    lineEdit->setText(m_document->textBuffer()->user()->name());
    widget->layout()->addWidget(lineEdit);
    dialog.setMainWidget(widget);
    lineEdit->setFocus();
    lineEdit->selectAll();
    if ( dialog.exec() == KDialog::Accepted ) {
        changeUserName(lineEdit->text());
    }
}

void KobbyPluginView::changeUserName(const QString& newUserName)
{
    kDebug() << "new user name" << newUserName;
    KUrl url = m_document->document()->url();
    url.setUser(newUserName);
    KTextEditor::Document* document = m_document->document();
    document->setModified(false);
    document->closeUrl();
    document->openUrl(url);
}

void KobbyPluginView::createServerActionClicked()
{

}

void KobbyPluginView::openActionClicked()
{
    KDialog* dialog = new KDialog(m_view);
    QWidget* widget = new QWidget(dialog);
    QFormLayout* layout = new QFormLayout();
    widget->setLayout(layout);

    QLineEdit* host = new QLineEdit();
    layout->addRow(new QLabel(i18n("Remote host address:")), host);

    QLineEdit* port = new QLineEdit("6523");
    layout->addRow(new QLabel(i18n("Port:")), port);

    QLineEdit* userName = new QLineEdit("UnnamedUser");
    layout->addRow(new QLabel(i18n("User name:")), userName);

    QLineEdit* password = new QLineEdit();
    layout->addRow(new QLabel(i18n("Password (optional):")), password);

    dialog->setMainWidget(widget);

    if ( dialog->exec() == KDialog::Accepted ) {
        KUrl url;
        url.setHost(host->text());
        url.setPort(port->text().toInt());
        url.setPath(QLatin1String("/"));
        url.setUser(userName->text());
        url.setPassword(password->text());
        url.setProtocol("inf");
        KUrl result = KFileDialog::getOpenUrl(url);
        if ( result.isValid() ) {
            m_view->document()->openUrl(KUrl(result));
        }
    }
    delete dialog;
}

void KobbyPluginView::saveCopyActionClicked()
{
    if ( ! m_document ) {
        return;
    }
    if ( m_document->localSavePath().isEmpty() ) {
        const QString saveUrl = KFileDialog::getSaveFileName();
        kDebug() << "saving to url" << saveUrl;
        if ( saveUrl.isEmpty() ) {
            return;
        }
        m_document->setLocalSavePath(saveUrl);
    }
    if ( ! m_document->saveCopy() ) {
        KMessageBox::error(m_view, i18n("Failed to save file to %1", m_document->localSavePath()));
        m_document->setLocalSavePath(QString());
    }
}

void KobbyPluginView::shareActionClicked()
{
    if ( ! m_view->document()->url().isValid() ) {
        const QString question = i18n("You must save the document before sharing it. Do you want to do that now?");
        if ( KMessageBox::questionYesNo(m_view, question) != KMessageBox::Yes ) {
            return;
        }
        QString saveName = KFileDialog::getSaveFileName();
        if ( saveName.isEmpty() ) {
            return;
        }
        if ( ! m_view->document()->saveAs(saveName) ) {
            return;
        }
    }
    Tp::registerTypes();
    ShareDocumentDialog dialog(m_view);
    connect(&dialog, SIGNAL(shouldOpenDocument(KUrl)),
            this, SLOT(openFile(KUrl)));
    dialog.exec();
}

void KobbyPluginView::openFile(KUrl url)
{
    kDebug() << "opening file" << url;
    m_view->document()->openUrl(url.url());
}

KobbyPluginView::~KobbyPluginView()
{
}

void KobbyPluginView::remoteTextChanged(const KTextEditor::Range range, QInfinity::User* user, bool removal)
{
    KConfig config("ktecollaborative");
    if ( config.group("notifications").readEntry("displayWidgets", true) ) {
        const QColor color = ColorHelper::colorForUsername(user->name(), m_view, m_document->changeTracker()->usedColors());
        RemoteChangeNotifier::addNotificationWidget(m_view, removal ? range.start() : range.end(),
                                                    user, color);
    }
}

void KobbyPluginView::documentReady(ManagedDocument* doc)
{
    Q_ASSERT(doc == m_document);
    connect(m_document->textBuffer(), SIGNAL(remoteChangedText(KTextEditor::Range,QInfinity::User*,bool)),
            this, SLOT(remoteTextChanged(KTextEditor::Range,QInfinity::User*,bool)));
    connect(m_document->userTable(), SIGNAL(userAdded(User*)),
            statusBar(), SLOT(usersChanged()));
    connect(m_document->userTable(), SIGNAL(userRemoved(User*)),
            statusBar(), SLOT(usersChanged()));
    m_statusBar->usersChanged();
    statusBar()->sessionFullyReady();

    enableActions();
}

#include "kobbypluginview.moc"
