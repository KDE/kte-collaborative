/*  This file is part of kobby
    Copyright (c) 2013 Sven Brauch <svenbrauch@gmail.com>


    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef __kio_infinity_h__
#define __kio_infinity_h__

#include <kio/global.h>
#include <kio/slavebase.h>

#include <kobby/editor/connection.h>
#include <libqinfinity/browsermodel.h>

class InfinityProtocol : public QObject, public KIO::SlaveBase
{
    Q_OBJECT

public:

    InfinityProtocol(const QByteArray &pool_socket, const QByteArray &app_socket);
    virtual ~InfinityProtocol();

    virtual void get(const KUrl& url);
    virtual void stat(const KUrl& url);

    virtual void mimetype(const KUrl& url);
    virtual void listDir(const KUrl& url);

    static InfinityProtocol* self();

private:
    static InfinityProtocol* _self;
    Kobby::Connection* m_connection;
    QInfinity::BrowserModel* m_browserModel;
};


#endif