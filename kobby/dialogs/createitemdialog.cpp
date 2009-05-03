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

#include "createitemdialog.h"

#include <QWidget>

#include "ui_createitemwidget.h"

namespace Kobby
{

CreateItemDialog::CreateItemDialog( QString title,
    QString label,
    QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CreateItemWidget )
{
    QWidget *mainWidget = new QWidget( this );
    ui->setupUi( mainWidget );
    ui->itemNameLabel->setText( label );
    setMainWidget( mainWidget );
    setWindowTitle( title );
}

QString CreateItemDialog::name() const
{
    return ui->nameLineEdit->text();
}

}

