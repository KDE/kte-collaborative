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

