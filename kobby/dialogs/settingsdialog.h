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

    private Q_SLOTS:
        void slotApplyClicked();

    private:
        void setupUi();
        void setupSignals();
        void saveSettings();

        KPageWidgetItem *profilePage;

};

}

#endif

