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

#ifndef KOBBYPLUGINVIEW_H
#define KOBBYPLUGINVIEW_H

#include <QObject>
#include <QLabel>
#include <QPushButton>
#include <KTextEditor/View>
#include <KUrl>

#include <libqinfinity/xmlconnection.h>
#include <libqinfinity/usertable.h>

#include "common/document.h"

class StatusOverlay;
namespace Kobby {
    class Connection;
}

namespace QInfinity {
    class User;
}

using Kobby::Connection;
using Kobby::Document;

class KAction;
class ManagedDocument;
class KteCollaborativePluginView;

/**
 * @brief Represents one pair of color box + user name, as used in the status bar.
 * This widget has a "collapsed" and an "expanded" state; in the former the
 * name label is hidden, and only the box is shown. It remembers how much size
 * it needs for its large state.
 */
class UserLabel : public QWidget {
Q_OBJECT
public:
    UserLabel(const QString& name, const QColor& color, bool online, QWidget* parent);

    /**
     * @brief Gives the difference between the collapsed and expanded size (i.e. size of label + margins)
     */
    int expandedIncrement() const { return m_labelIncrement; };

    /**
     * @brief Sets this widget to be expanded or collapsed, hiding or showing the name label.
     */
    void setExpanded(bool expanded);

private:
    /// The pixmap used to display the user's color
    QPixmap box;
    /// @see expandedIncrement()
    int m_labelIncrement;
    QLabel* m_nameLabel;
};

/**
 * @brief A widget displaying a list of users next to each other.
 * It has three modes:
 *   - Expanded: User names and colors are displayed next to each other
 *   - Collapsed: Only the colors are displayed, names are provided by tooltips
 *   - "too many users": For >25 users, just displays the amount of users.
 * The widget will automatically switch to the best of the first two modes, depending on how
 * much size is available. It uses the given view for these calculations. Switching occurs when the
 * widget is resized, or when the user table changes.
 */
class HorizontalUsersList : public QWidget {
Q_OBJECT
public:
    HorizontalUsersList(KteCollaborativePluginView* view, QWidget* parent = 0, Qt::WindowFlags f = 0);

    /**
     * @brief Provide the user table to the widget. It will not do anything before you call this.
     */
    void setUserTable(QInfinity::UserTable* table);

    /**
     * @brief Remove all user labels from the widget.
     */
    void clear();

    /**
     * @brief Put the widget into expanded or collapsed mode
     * @param expanded true for displaying name labels, false for only color boxes
     */
    void setExpanded(bool expanded);

    /**
     * @brief Retrieve the size this widget would take if it was expanded.
     * This is fast. The return value is undefined if the widget is currently expanded.
     * @return int size (in pixels) of this widget in expanded mode.
     */
    int expandedSize() const;

public slots:
    /**
     * @brief Invoked when the user table changes, i.e. user name change, user joined/left
     */
    void userTableChanged();
    /// Handlers for the button menu actions
    void showOffline(bool showOffline);
    void showIncative(bool showInactive);

signals:
    void needSizeCheck();

private:
    /**
     * @brief Add a label for the given user to the list.
     * @param name Name for the user. Color is determined from this.
     * @param displayName The name which is actually displayed.
     */
    void addLabelForUser(const QString& name, bool online, const QString& displayName);

private:
    QInfinity::UserTable* m_userTable;
    /// Text in front of the user list, e.g. "Users:"
    QPushButton* m_prefix;
    KteCollaborativePluginView* m_view;
    QList<UserLabel*> m_userLabels;
    bool m_isExpanded;
    bool m_showInactive;
    bool m_showOffline;
};

/**
 * @brief This class manages the status bar below a collaborative document.
 * It is responsible for updating the text in the status bar itself.
 */
class CollaborativeStatusBar : public QWidget {
Q_OBJECT
public:
    explicit CollaborativeStatusBar(KteCollaborativePluginView* parent, Qt::WindowFlags f = 0);
    virtual bool event(QEvent* );

public slots:
    void connectionStatusChanged(Connection*, QInfinity::XmlConnection::Status status);
    void sessionFullyReady();
    void checkSize();

    /**
     * @brief Should be invoked when something about the users for a document changed.
     * This includes both user count and any user's properties (name, status, ...)
     */
    void usersChanged();

private:
    QLabel* m_connectionStatusLabel;
    KteCollaborativePluginView* m_view;
    HorizontalUsersList* m_usersList;
};

/**
 * @brief This class manages the UI for a collaborative document.
 */
class KteCollaborativePluginView
    : public QObject
    , public KXMLGUIClient
{
Q_OBJECT
public:
    /**
     * @brief Adds the plugin's user interface to a KTextEditor::View, and keeps it up-to-date.
     *
     * @param kteView The KTextEditor::View instance to add the UI to. Usually you get this from the plugin's addView() function.
     * @param document The ManagedDocument instance used for this view, needed for setting up connections.
     */
    KteCollaborativePluginView(KTextEditor::View* kteView, ManagedDocument* document);
    virtual ~KteCollaborativePluginView();

    /**
     * @brief Get the status bar instance used for this view
     */
    CollaborativeStatusBar* statusBar() const;

    /**
     * @brief Get the document this view is for
     */
    ManagedDocument* document() const;

    /**
     * @brief Get the KTextEditor::View this view is for
     */
    KTextEditor::View* kteView() const;

public slots:
    /**
     * @brief Should be invoked when a remote (only remote) user changed text.
     * It will add a small popup widget indicating the user name which typed some text to the view.
     * @param range The range of the text which changed
     * @param user The remote user which changed the text
     * @param removal true if the text was removed, false if it was inserted
     */
    void remoteTextChanged(const KTextEditor::Range range, QInfinity::User* user, bool removal);
    void documentBecameManaged(ManagedDocument*);
    void documentBecameUnmanaged(ManagedDocument*);
    void documentReady(ManagedDocument*);

    void disconnectActionClicked();
    void openActionClicked();
    void saveCopyActionClicked();
    void shareActionClicked();
    void changeUserActionClicked();
    void clearHighlightActionClicked();
    void configureActionClicked();
    void openFileManagerActionClicked();

    void openFile(KUrl);

    void textHintRequested(const KTextEditor::Cursor& position, QString& hint);

private:
    KTextEditor::View* m_view;
    CollaborativeStatusBar* m_statusBar;
    StatusOverlay* m_statusOverlay;
    ManagedDocument* m_document;

    // actions from the "Collaborative" menu
    KAction* m_openCollabDocumentAction;
    KAction* m_saveCopyAction;
    KAction* m_shareWithContactAction;
    KAction* m_changeUserNameAction;
    KAction* m_disconnectAction;
    KAction* m_configureAction;
    KAction* m_openFileManagerAction;

    // actions from the popup menu
    KAction* m_clearHighlightAction;

    // list of actions which should only be enabled when a document
    // is a connected collaborative document
    QList<KAction*> m_actionsRequiringConnection;

    // Enables or disables the actions in m_actionsRequiringConnection
    void enableActions();
    void disableActions();

    // Enables or disables the UI (status bar, tracking widget, ...)
    void enableUi();
    void disableUi();

    /**
     * @brief Changes the document's user name to the given name.
     */
    void changeUserName(const QString& newUserName);

    friend class CollaborativeStatusBar;
};

#endif // KOBBYPLUGINVIEW_H
