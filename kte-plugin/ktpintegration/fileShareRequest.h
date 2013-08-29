/***************************************************************************
 *   Copyright (C) 2013 by Sven Brauch <svenbrauch@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for More details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#ifndef FILE_SHARE_REQUEST_H
#define FILE_SHARE_REQUEST_H

#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/Constants>
#include <TelepathyQt/Account>
#include <TelepathyQt/ReferencedHandles>

#include <KUrl>
#include <KDebug>

typedef QList<KUrl> DocumentList;

class FileShareRequest {
    private:
    static const QVariantMap createHints(const DocumentList& documents)
    {
        QVariantMap hints;
        hints.insert(QLatin1String("initialDocumentsSize"), documents.size());
        for ( int i = 0; i < documents.size(); i++ ) {
            hints.insert(QLatin1String("initialDocument") + QString::number(i), documents.at(i).fileName());
        }
        return hints;
    }

    static Tp::PendingChannelRequest* createRequest(const Tp::AccountPtr account, const DocumentList documents, QVariantMap requestBase)
    {
        QVariantMap hints = createHints(documents);

        requestBase.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                        TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE);
        requestBase.insert(TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE + QLatin1String(".Service"),
                        QLatin1String("infinote"));

        Tp::PendingChannelRequest* channelRequest;
        channelRequest = account->ensureChannel(requestBase,
                                                QDateTime::currentDateTime(),
                                                QLatin1String("org.freedesktop.Telepathy.Client.KTp.infinoteServer"),
                                                hints);

        return channelRequest;
    }

public:
    static Tp::PendingChannelRequest* offer(const Tp::AccountPtr& account, const Tp::ContactPtr& contact, const DocumentList& documents)
    {
        kDebug() << "share with account requested";
        QVariantMap request;
        request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                    (uint) Tp::HandleTypeContact);
        request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetHandle"),
                    contact->handle().at(0));
        return createRequest(account, documents, request);
    }

    static Tp::PendingChannelRequest* offer(const Tp::AccountPtr& account, const QString& chatroom, const DocumentList& documents)
    {
        kDebug() << "share with chatroom" << chatroom << "requested";
        QVariantMap request;
        request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                    (uint) Tp::HandleTypeRoom);
        request.insert(TP_QT_IFACE_CHANNEL + QLatin1String(".TargetID"),
                    chatroom);
        return createRequest(account, documents, request);
    }

    static Tp::PendingChannelRequest* offer(const Tp::AccountPtr& /*account*/, const Tp::Contacts& /*contact*/, const DocumentList& /*documents*/)
    {
        kWarning() << "not implemented";
        return 0;
    }
};

#endif