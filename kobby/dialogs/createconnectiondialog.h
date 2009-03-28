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
