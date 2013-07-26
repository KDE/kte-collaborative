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
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "inftube.h"
#include <krun.h>
#include <KDebug>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KApplication>
#include <TelepathyQt/ClientRegistrar>

// Proxy application which accepts a tube, then starts a handler application.
int main(int argc, char** argv) {
    KAboutData about("infinote-client", 0, ki18n("infinote-client"), "0.1", ki18n("..."),
                     KAboutData::License_GPL, ki18n("(C) 2013 Sven Brauch"), KLocalizedString(), 0, "svenbrauch@gmail.com");
    about.addAuthor( ki18n("Sven Brauch"), KLocalizedString(), "svenbrauch@gmail.com" );
    KCmdLineArgs::init(argc, argv, &about);
    kDebug() << "Starting tube acceptor";

    KApplication app;
    InfTubeClient* client = new InfTubeClient();
    client->listen();
    app.exec();
    delete client;
}