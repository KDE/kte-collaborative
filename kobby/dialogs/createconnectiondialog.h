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
    Q_OBJECT

    public:
        CreateConnectionDialog( QWidget *parent = 0 );
    
    private Q_SLOTS:
        void slotOkClicked();

    private:
        void setupActions();
        
        Ui::CreateConnectionWidget *ui;
};

}

#endif
