#ifndef KOBBY_SESSIONMANAGER_H
#define KOBBY_SESSIONMANAGER_H

#include <KDialog>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "ui_connectionmanager.h"
#include "addconnectiondialog.h"

namespace Kobby
{

class ConnectionManager
    : public KDialog
{
    
    Q_OBJECT
    
    public:
        ConnectionManager( QWidget *parent = 0 );
        ~ConnectionManager();
    
    public Q_SLOTS:
        void slotAddConnection();
        void slotAddConnectionFinished();
    
    private:
        void setupActions();
        
        AddConnectionDialog *addConnectionDialog;
        Ui::ConnectionManager ui;
    
}; // class ConnectionManager

} // namespace Kobby

#endif
