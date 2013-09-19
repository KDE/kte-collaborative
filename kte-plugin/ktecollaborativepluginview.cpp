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

#include "ktecollaborativepluginview.h"
#include "manageddocument.h"
#include "ktecollaborativeplugin.h"
#include "ui/remotechangenotifier.h"
#include "ui/sharedocumentdialog.h"
#include "ui/opencollabdocumentdialog.h"
#include "ui/statusoverlay.h"
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
#include <QBoxLayout>
#include <QCommandLinkButton>
#include <QPaintEngine>
#include <QMenu>

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

HorizontalUsersList::HorizontalUsersList(KteCollaborativePluginView* view, QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_userTable(0)
    , m_prefix(new QPushButton(this))
    , m_view(view)
    , m_showInactive(true)
    , m_showOffline(false)
{
    setLayout(new QHBoxLayout);
    layout()->addWidget(m_prefix);
    m_prefix->setFlat(true);

    // Generate the dropdown menu for the "Users:" button
    QMenu* menu = new QMenu(m_prefix);
    QAction* showOfflineAction = new QAction(KIcon("im-user-away"), i18n("Show offline users"), m_prefix);
    showOfflineAction->setCheckable(true);
    showOfflineAction->setChecked(m_showOffline);
    QAction* showInactiveAction = new QAction(KIcon("im-invisible-user"), i18n("Show users without contributions"), m_prefix);
    showInactiveAction->setCheckable(true);
    showInactiveAction->setChecked(m_showInactive);
    menu->addAction(showOfflineAction);
    menu->addAction(showInactiveAction);
    m_prefix->setMenu(menu);

    connect(showOfflineAction, SIGNAL(triggered(bool)), this, SLOT(showOffline(bool)));
    connect(showInactiveAction, SIGNAL(triggered(bool)), this, SLOT(showIncative(bool)));

    // Disable this option when background highlighting is turned off, since it doesn't make much sense.
    // Also it's difficult to implement.
    KConfig config("ktecollaborative");
    showInactiveAction->setEnabled(config.group("notifications").readEntry("highlightBackground", true));
}

void HorizontalUsersList::showOffline(bool showOffline)
{
    m_showOffline = showOffline;
    userTableChanged();
}

void HorizontalUsersList::showIncative(bool showInactive)
{
    m_showInactive = showInactive;
    userTableChanged();
}

void HorizontalUsersList::setUserTable(QInfinity::UserTable* table)
{
    m_userTable = table;
}

void HorizontalUsersList::clear()
{
    qDeleteAll(m_userLabels);
    m_userLabels.clear();
}

void HorizontalUsersList::addLabelForUser(const QString& name, bool online, const QString& displayName)
{
    if ( name == "Initial document contents" ) return; // TODO urgh
    const QColor& color = ColorHelper::colorForUsername(name, m_view->kteView(),
                                                        m_view->document()->changeTracker()->usedColors());
    UserLabel* label = new UserLabel(displayName, color, online, this);
    m_userLabels.append(label);
    if ( online ) {
        // Sort online users to the front
        qobject_cast<QBoxLayout*>(layout())->insertWidget(1, label);
    }
    else {
        layout()->addWidget(label);
    }
}

UserLabel::UserLabel(const QString& name, const QColor& color, bool online, QWidget* parent)
    : QWidget(parent)
    , box(QSize(12, 12))
{
    setLayout(new QHBoxLayout);
    QColor saturated(color);
    saturated.setHsv(color.hsvHue(), qMin(255, (int) (color.hsvSaturation() * 1.5)), color.value());

    // draw the pixmap with the users' color
    QLabel* colorBox = new QLabel();
    QPainter p(&box);
    p.setBrush(QBrush(saturated));
    p.setPen(QPen(saturated));
    p.drawRect(0, 0, 12, 12);
    // draw a nice shadow
    p.setPen(QPen(saturated.darker(140)));
    p.drawRect(0, 0, 11, 11);
    p.setPen(QPen(saturated.darker(125)));
    p.drawRect(1, 1, 9, 9);
    p.setPen(QPen(saturated.darker(110)));
    p.drawRect(2, 2, 7, 7);

    if ( ! online ) {
        // If the user is offline, draw an extra shadow mark.
        QVector<QPoint> points;
        points << QPoint(0, 12) << QPoint(12, 0) << QPoint(12, 12);
        p.setPen(QPen(saturated.darker(160)));
        p.setBrush(QBrush(saturated.darker(160)));
        p.drawPolygon(points.data(), points.size());
    }

    colorBox->setPixmap(box);
    layout()->addWidget(colorBox);

    // remember the size before adding the label
    const int small = sizeHint().width();
    m_nameLabel = new QLabel(name);
    layout()->addWidget(m_nameLabel);
    // the difference between the old and the new size gives the increment which would
    // occur if the label was displayed.
    m_labelIncrement = sizeHint().width() - small;
    // do not display the label initially to avoid flicker
    m_nameLabel->setVisible(false);

    colorBox->setToolTip(name);
}

void UserLabel::setExpanded(bool expanded)
{
    m_nameLabel->setVisible(expanded);
}

void HorizontalUsersList::userTableChanged()
{
    if ( ! m_userTable || ! m_view->document()->textBuffer()->user() ) {
        return;
    }
    clear();

    const QString& ownUserName = m_view->document()->textBuffer()->user()->name();
    QList< QPointer< QInfinity::User > > users = m_userTable->users();
    const int activeUsers = m_view->document()->changeTracker()->usedColors().count();
    foreach ( const QPointer<QInfinity::User>& user, users ) {
        connect(user.data(), SIGNAL(statusChanged()), this, SLOT(userTableChanged()), Qt::UniqueConnection);
    }

    if ( ! m_showOffline ) {
        users = m_userTable->activeUsers();
    }
    // If more than 20 users would be displayed, just display how many it would be, for performance
    // and size reasons.
    if ( users.length() > 20 && ( m_showInactive || ( ! m_showInactive && activeUsers > 20 ) ) ) {
        m_prefix->setText(i18nc("tells how many users are online", "%1 users (%2 online)",
                                m_userTable->users().size(), m_userTable->activeUsers().size()));
        return;
    }

    // Otherwise, generate the full table.
    m_prefix->setText(i18n("Users:"));
    foreach ( const QPointer<QInfinity::User>& user, users ) {
        if ( ! m_showInactive && ! m_view->document()->changeTracker()->usedColors().contains(user->name()) ) {
            // No color was generated for the user; that would have happened if he had wrote text.
            continue;
        }
        QString label((user->name() == ownUserName) ? i18nc("%1 is your name", "%1 (you)", user->name()) : user->name());
        label = (user->status() == QInfinity::User::Active) ? label : i18nc("%1 is a name", "%1 (offline)", label);
        addLabelForUser(user->name(), user->status() == QInfinity::User::Active, label);
    }
    emit needSizeCheck();
}

int HorizontalUsersList::expandedSize() const
{
    // The size the widget would have if expanded is the current (small) size
    // plus all the individual increments caused by displaying the user names.
    int increment = 0;
    foreach ( const UserLabel* label, m_userLabels ) {
        increment += label->expandedIncrement();
    }
    return sizeHint().width() + increment;
}

void HorizontalUsersList::setExpanded(bool expanded)
{
    m_isExpanded = expanded;
    foreach ( UserLabel* label, m_userLabels ) {
        label->setExpanded(expanded);
    }
}

CollaborativeStatusBar::CollaborativeStatusBar(KteCollaborativePluginView* parent, Qt::WindowFlags f)
    : QWidget(parent->m_view, f)
    , m_connectionStatusLabel(new QLabel(this))
    , m_view(parent)
    , m_usersList(new HorizontalUsersList(m_view))
{
    setLayout(new QHBoxLayout());
    layout()->setAlignment(Qt::AlignRight);
    layout()->addWidget(m_usersList);
    layout()->addWidget(m_connectionStatusLabel);
    QTimer::singleShot(0, this, SLOT(checkSize()));
    connect(m_usersList, SIGNAL(needSizeCheck()), SLOT(checkSize()));
    connect(m_view->document()->changeTracker(), SIGNAL(colorTableChanged()),
            m_usersList, SLOT(userTableChanged()));
}

bool CollaborativeStatusBar::event(QEvent* e)
{
    if ( e->type() == QEvent::Resize ) {
        checkSize();
    }
    return QWidget::event(e);
}

void CollaborativeStatusBar::checkSize()
{
    int availableSize = m_view->kteView()->size().width() - m_connectionStatusLabel->size().width();
    int actualSize = m_usersList->sizeHint().width();
    if ( availableSize - actualSize < 25 ) {
        m_usersList->setExpanded(false);
    }
    else if ( availableSize - m_usersList->expandedSize() > 35 ) {
        m_usersList->setExpanded(true);
    }
}

void CollaborativeStatusBar::connectionStatusChanged(Kobby::Connection*, QInfinity::XmlConnection::Status status)
{
    QString text;
    KColorScheme::ForegroundRole role = KColorScheme::NormalText;
    if ( status == QInfinity::XmlConnection::Closed ) {
        // Since the connection will start opening immediately,
        // this will not display in the beginning, just on disconnect
        text = "<b>" + i18n("Disconnected from collaboration server.") + "</b>";
        role = KColorScheme::NegativeText;
        m_usersList->clear();
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

ManagedDocument* KteCollaborativePluginView::document() const
{
    return m_document;
}

KTextEditor::View* KteCollaborativePluginView::kteView() const
{
    return m_view;
}

void CollaborativeStatusBar::sessionFullyReady()
{
    setTextColor(m_connectionStatusLabel, KColorScheme::PositiveText);
    m_connectionStatusLabel->setText( "<b>" + i18n("Connected to collaboration server.") + "</b>" );
}

void CollaborativeStatusBar::usersChanged()
{
    m_usersList->setUserTable(m_view->m_document->userTable());
    m_usersList->userTableChanged();
    checkSize();
}

CollaborativeStatusBar* KteCollaborativePluginView::statusBar() const
{
    return m_statusBar;
}

KteCollaborativePluginView::KteCollaborativePluginView(KTextEditor::View* kteView, ManagedDocument* document)
    : QObject(kteView)
    , KXMLGUIClient(kteView)
    , m_view(kteView)
    , m_statusBar(0)
    , m_statusOverlay(0)
    , m_document(document)
{
    setComponentData(KteCollaborativePluginFactory::componentData());
    setXMLFile("ktexteditor_collaborativeui.rc");

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

void KteCollaborativePluginView::openFileManagerActionClicked()
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

void KteCollaborativePluginView::configureActionClicked()
{
    KCMultiDialog proxy;
    proxy.addModule("ktexteditor_collaborative_config");
    proxy.exec();
}

void KteCollaborativePluginView::clearHighlightActionClicked()
{
    if ( m_document && m_document->changeTracker() ) {
        m_document->changeTracker()->clearHighlight();
    }
}

void KteCollaborativePluginView::disableActions()
{
    foreach ( KAction* action, m_actionsRequiringConnection ) {
        action->setEnabled(false);
    }
    if ( QAction* save = m_view->action("file_save") ) {
        save->setEnabled(true);
    }
}

void KteCollaborativePluginView::enableActions()
{
    foreach ( KAction* action, m_actionsRequiringConnection ) {
        action->setEnabled(true);
    }
    // When enabling the collaborative actions,
    // we want to disable the built-in save and undo.
    // TODO make Ctrl+S call the "Save copy" action somehow?
    if ( QAction* save = m_view->action("file_save") ) {
        save->setEnabled(false);
    }
}

void KteCollaborativePluginView::documentBecameManaged(ManagedDocument* document)
{
    if ( document->document() != m_view->document() ) {
        return;
    }
    m_document = document;
    enableUi();
}

void KteCollaborativePluginView::documentBecameUnmanaged(ManagedDocument* document)
{
    if ( document != m_document || m_document == 0 ) {
        return;
    }
    Q_ASSERT(document->document() == m_document->document());
    m_document = 0;
    disableUi();
    disableActions();
}

void KteCollaborativePluginView::textHintRequested(const KTextEditor::Cursor& position, QString& hint)
{
    hint = i18nc("%1 is a user name", "Written by: <b>%1</b>", m_document->changeTracker()->userForCursor(position));
}

void KteCollaborativePluginView::enableUi()
{
    Document::LoadState loadState = document()->infTextDocument() ? document()->infTextDocument()->loadState()
                                                                  : Kobby::Document::Unloaded;
    if ( loadState != Kobby::Document::Complete ) {
        m_statusOverlay = new StatusOverlay(m_view);
        m_statusOverlay->move(0, 0);
        connect(m_document->connection(), SIGNAL(statusChanged(Connection*,QInfinity::XmlConnection::Status)),
                m_statusOverlay, SLOT(connectionStatusChanged(Connection*,QInfinity::XmlConnection::Status)));
        connect(m_document, SIGNAL(loadStateChanged(Document*,Document::LoadState)),
                m_statusOverlay, SLOT(loadStateChanged(Document*,Document::LoadState)));
        connect(m_document, SIGNAL(synchroinzationProgress(double)),
                m_statusOverlay, SLOT(progress(double)));
        m_statusOverlay->show();
    }

    m_statusBar = new CollaborativeStatusBar(this);
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

    if ( loadState == Kobby::Document::Complete ) {
        statusBar()->sessionFullyReady();
        statusBar()->usersChanged();
    }
}

void KteCollaborativePluginView::disableUi()
{
    m_view->layout()->removeWidget(m_statusBar);
    delete m_statusBar;
    m_statusBar = 0;

    delete m_statusOverlay;
    m_statusOverlay = 0;

    m_shareWithContactAction->setEnabled(true);
    if ( KTextEditor::TextHintInterface* iface = qobject_cast<KTextEditor::TextHintInterface*>(m_view) ) {
        iface->disableTextHints();
    }
    // Connections are disconnected automatically since m_document will be deleted
}

void KteCollaborativePluginView::disconnectActionClicked()
{
    m_document->document()->saveAs(KUrl(QDir::tempPath() + m_document->document()->url().encodedPath()));
}

void KteCollaborativePluginView::changeUserActionClicked()
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

void KteCollaborativePluginView::changeUserName(const QString& newUserName)
{
    kDebug() << "new user name" << newUserName;
    KUrl url = m_document->document()->url();
    url.setUser(newUserName);
    KTextEditor::Document* document = m_document->document();
    document->setModified(false);
    document->closeUrl();
    document->openUrl(url);
}

void KteCollaborativePluginView::openActionClicked()
{
    OpenCollabDocumentDialog* dialog = new OpenCollabDocumentDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, SIGNAL(shouldOpenDocument(KUrl)),
            m_view->document(), SLOT(openUrl(KUrl)));
    dialog->show();
}

void KteCollaborativePluginView::saveCopyActionClicked()
{
    if ( ! m_document ) {
        return;
    }
    // Suggest the original URL of the document in the save dialog.
    // Not necessarily valid, but that doesn't hurt.
    KUrl suggestedUrl(m_document->document()->property("oldUrl").toString());
    if ( m_document->localSavePath().isEmpty() ) {
        const QString saveUrl = KFileDialog::getSaveFileName(suggestedUrl);
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

void KteCollaborativePluginView::shareActionClicked()
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

void KteCollaborativePluginView::openFile(KUrl url)
{
    kDebug() << "opening file" << url;
    m_view->document()->setProperty("oldUrl", m_view->document()->url().url());
    m_view->document()->openUrl(url.url());
}

KteCollaborativePluginView::~KteCollaborativePluginView()
{
}

void KteCollaborativePluginView::remoteTextChanged(const KTextEditor::Range range, QInfinity::User* user, bool removal)
{
    KConfig config("ktecollaborative");
    if ( config.group("notifications").readEntry("displayWidgets", true) ) {
        const QColor color = ColorHelper::colorForUsername(user->name(), m_view, m_document->changeTracker()->usedColors());
        RemoteChangeNotifier::addNotificationWidget(m_view, removal ? range.start() : range.end(),
                                                    user, color);
    }
}

void KteCollaborativePluginView::documentReady(ManagedDocument* doc)
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
    m_statusOverlay = 0; // will delete itself

    enableActions();
}

#include "ktecollaborativepluginview.moc"
