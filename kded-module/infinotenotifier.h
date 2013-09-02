/*
 * Copyright 2013 Sven Brauch <svenbrauch@gmail.com>
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

#ifndef INFINOTENOTIFIER_H
#define INFINOTENOTIFIER_H

#include <QtDBus/QDBusContext>

#include <kdedmodule.h>

class OrgKdeKDirNotifyInterface;
class InfinoteNotifier : public KDEDModule, QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.InfinoteNotifier")

public:
    InfinoteNotifier(QObject *parent, const QVariantList &);
    OrgKdeKDirNotifyInterface* m_notifyIface;

private slots:
    void enteredDirectory(QString);
    void leftDirectory(QString);
};

#endif // INFINOTENOTIFIER_H
