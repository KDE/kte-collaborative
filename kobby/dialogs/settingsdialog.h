/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
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

#ifndef KOBBY_SETTINGSDIALOG_H
#define KOBBY_SETTINGSDIALOG_H

#include <KConfigDialog>

class KPageWidgetItem;

namespace Kobby
{

class SettingsDialog
    : public KConfigDialog
{

    public:
        SettingsDialog( QWidget *parent = 0 );

    private:
        void setupUi();

        KPageWidgetItem *profilePage;
        KPageWidgetItem *networkPage;

};

}

#endif

