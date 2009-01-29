#include <QWidget>

namespace Ui
{
    class ProfileSettingsWidget;
}

namespace Kobby
{

class ProfileSettings
    : public QWidget
{

    public:
        ProfileSettings( QWidget *parent = 0 );
    
    private:
        Ui::ProfileSettingsWidget *ui;
    
};

}

