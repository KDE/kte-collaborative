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

#ifndef KOBBY_CREATECONNECTIONDIALOG_H
#define KOBBY_CREATECONNECTIONDIALOG_H

#include <KDialog>

namespace Ui
{
    class CreateConnectionWidget;
}

namespace Kobby
{

class CreateConnectionDialog
    : public KDialog
{
    Q_OBJECT;

    public:
        CreateConnectionDialog( QWidget *parent = 0 );

    Q_SIGNALS:
        void createConnection( const QString &hostname,
            unsigned int port );

    private Q_SLOTS:
        void slotOkClicked();

    private:
        void setupActions();
        bool verifyInput();
        
        Ui::CreateConnectionWidget *ui;
};

}

#endif
