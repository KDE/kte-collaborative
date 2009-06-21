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

#ifndef KOBBY_SETUP_DIALOG_H
#define KOBBY_SETUP_DIALOG_H

#include <KAssistantDialog>

#include <QWidget>

class QLineEdit;

namespace Ui
{
    class ProfileSetupWidget;
    class SetupPageWidget;
}

namespace Kobby
{

class SetupDialog
    : public KAssistantDialog
{
    Q_OBJECT

    public:
        SetupDialog( QWidget *parent = 0 );

    private Q_SLOTS:
        void slotFinished();
        void slotProfileTextEntered( const QString &text );

    private:
        Ui::ProfileSetupWidget *profileUi;
        Ui::SetupPageWidget *setupUi;
        QWidget *profilePage;
        KPageWidgetItem *profilePageItem;

};

}

#endif
