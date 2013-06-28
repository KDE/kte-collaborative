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
#include <KTextEditor/View>

#include <libqinfinity/xmlconnection.h>

#include "common/document.h"

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
class KobbyPluginView;

/**
 * @brief This class manages the status bar below a collaborative document.
 * It is responsible for updating the text in the status bar itself.
 */
class KobbyStatusBar : public QWidget {
Q_OBJECT
public:
    explicit KobbyStatusBar(KobbyPluginView* parent, Qt::WindowFlags f = 0);

public slots:
    void connectionStatusChanged(Connection*, QInfinity::XmlConnection::Status status);
    void sessionFullyReady();

    /**
     * @brief Should be invoked when something about the users for a document changed.
     * This includes both user count and any user's properties (name, status, ...)
     */
    void usersChanged();

private:
    QLabel* m_connectionStatusLabel;
    QLabel* m_usersLabel;
    KobbyPluginView* m_view;
};

/**
 * @brief This class manages the UI for a collaborative document.
 */
class KobbyPluginView
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
    KobbyPluginView(KTextEditor::View* kteView, ManagedDocument* document);
    virtual ~KobbyPluginView();

    /**
     * @brief Get the status bar instance used for this view
     */
    KobbyStatusBar* statusBar() const;

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
    void createServerActionClicked();
    void clearHighlightActionClicked();
    void configureActionClicked();

private:
    KTextEditor::View* m_view;
    KobbyStatusBar* m_statusBar;
    ManagedDocument* m_document;

    // actions from the "Collaborative" menu
    KAction* m_openCollabDocumentAction;
    KAction* m_saveCopyAction;
    KAction* m_shareWithContactAction;
    KAction* m_changeUserNameAction;
    KAction* m_disconnectAction;
    KAction* m_createServerAction;
    KAction* m_configureAction;

    // actions from the popup menu
    KAction* m_clearHighlightAction;

    // Enables or disables the UI (status bar, tracking widget, ...)
    void enableUi();
    void disableUi();

    /**
     * @brief Changes the document's user name to the given name.
     */
    void changeUserName(const QString& newUserName);

    friend class KobbyStatusBar;
};

#endif // KOBBYPLUGINVIEW_H
