#ifndef KOBBY_SESSIONMANAGER_H
#define KOBBY_SESSIONMANAGER_H

#include <KDialog>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "ui_connectionmanager.h"
#include "addconnectiondialog.h"

namespace Infinity
{
    class XmppConnection;
}

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
        void slotAddConnectionDialog();
        void slotAddConnectionDialogFinished();
        void slotAddConnection( Infinity::XmppConnection &conn, const QString &hostname );
    
    private:
        void setupActions();
        
        AddConnectionDialog *addConnectionDialog;
        Ui::ConnectionManager ui;
    
}; // class ConnectionManager

} // namespace Kobby

#endif
