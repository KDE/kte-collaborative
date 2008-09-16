// This file may be licensed under the terms of of the
// GNU General Public License Version 2 (the "GPL").
// 
// Software distributed under the License is distributed 
// on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
// express or implied. See the GPL for the specific language 
// governing rights and limitations.
//
// You should have received a copy of the GPL along with this 
// program. If not, go to http://www.gnu.org/licenses/gpl.html
// or write to the Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#ifndef KOBBY_CONTROLDIALOG_H
#define KOBBY_CONTROLDIALOG_H

#include <KDialog>

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>

namespace Ui
{
    class ControlWidget;
}

namespace Kobby
{

class InfinoteManager;
class ConnectionListWidget;
class ConnectionManagerWidget;

/**
 * @brief A control interface for managing the kobby connections.
 */
class ControlWidget
    : public QWidget
{

    public:
        ControlWidget( InfinoteManager &infinoteManager, QWidget *parent = 0 );

    private:
        void setupUi();
        void setupActions();

        InfinoteManager *infinoteManager;
        ConnectionManagerWidget *connectionManagerWidget;
        QPushButton *addConnectionButton;
        QPushButton *removeConnectionButton;
        Ui::ControlWidget *ui;

};

/**
 * @brief A dialog containing a ControlWidget.
 */
class ControlDialog
    : public KDialog
{

    public:
        ControlDialog( InfinoteManager &infinoteManager, QWidget *parent = 0 );

    private:
        void setupUi();

        InfinoteManager *infinoteManager;
        ControlWidget *controlWidget;

};

}

#endif
