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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QList>

#include <ktexteditor/plugin.h>

namespace Kobby
{

class Plugin
    : public KTextEditor::Plugin
{
public:
    explicit Plugin(QObject *parent = 0,
        const QVariantList &args = QVariantList());

    void addDocument(KTextEditor::Document *document);
    void removeDocument(KTextEditor::Document *document);
    void addView(KTextEditor::View *view);
    void removeView(KTextEditor::View *view);

private:
    QList<class View*> m_views;
    QList<class Document*> m_documents;

}; // class Plugin

K_PLUGIN_FACTORY_DECLARATION(PluginFactory)

} // namespace Kobby

#endif

