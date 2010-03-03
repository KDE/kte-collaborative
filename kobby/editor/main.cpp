/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 * Copyright 2009  Ryan Kavanagh <ryanakca@kubuntu.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "setupdialog.h"
#include "kobbysettings.h"

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocalizedString>


#include <libqinfinity/init.h>

int main( int argc, char **argv )
{
    int ret;
    KAboutData aboutData( "kobby", 0,
        ki18n( "Kobby" ), "1.0 RC 1",
        ki18n( "Collaborative text editing environment." ),
        KAboutData::License_GPL,
        ki18n( "Copyright (c) 2008-2010 Gregory Haynes" ),
        KLocalizedString(),
        "http://kobby.greghaynes.net", "kobby@groups.google.com" );
    aboutData.addAuthor( ki18n("Gregory Haynes"), ki18n( "Creator, Maintainer" ), "greg@greghaynes.net" );
    
    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication app;

    QInfinity::init();
    Kobby::MainWindow *mainWindow = new Kobby::MainWindow();
    mainWindow->show();
    
    ret = app.exec();
    QInfinity::deinit();
    return ret;
}
