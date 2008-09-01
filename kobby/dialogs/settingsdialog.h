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

#ifndef KOBBY_SETTINGSDIALOG_H
#define KOBBY_SETTINGSDIALOG_H

#include <kobby/editor/plugin.h>

#include <KDialog>
#include <KCModule>

namespace Ui
{
    class SettingsWidget;
    class UserSettingsWidget;
}

namespace Kobby
{

class SettingsDialog
    : public KCModule
{

    public:
        SettingsDialog( QWidget *parent = 0, const QVariantList &args = QVariantList() );

    private:
        Ui::SettingsWidget *settingsWidgetUi;
        Ui::UserSettingsWidget *userSettingsWidgetUi;

};

}

#endif
