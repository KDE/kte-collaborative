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

#ifndef KOBBY_CONTROL_WIDGET
#define KOBBY_CONTROL_WIDGET

#include "infinotemanager.h"
#include "connectionmanager.h"
#include "filebrowser.h"

#include <QWidget>
#include <QSplitter>
#include <QTabWidget>

namespace Kobby
{

class ControlWidget
    : public QWidget
{

    public:
        ControlWidget( InfinoteManager &manager, QWidget *parent = 0 );

    private:
        InfinoteManager *infinoteManager;
        QSplitter splitter;
        ConnectionManager *connectionManager;
        QTabWidget filebrowserTabs;

}; // class ControlWidget

}

#endif
