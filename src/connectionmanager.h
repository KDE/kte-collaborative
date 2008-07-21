#ifndef KOBBY_SESSIONMANAGER_H
#define KOBBY_SESSIONMANAGER_H

#include "addconnectiondialog.h"
#include "infinotemanager.h"

#include <KDialog>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "ui_connectionmanager.h"

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
        ConnectionManager( InfinoteManager &manager, QWidget *parent = 0 );
        ~ConnectionManager();
    
    public Q_SLOTS:
        void addConnection( Infinity::XmppConnection &conn, const QString &hostname );
        void addConnection( const QString hostname, unsigned int port );
    
    private Q_SLOTS:
        void slotAddConnectionDialog();
        void slotAddConnectionDialogFinished();
    
    private:
        void setupActions();
        
        AddConnectionDialog *addConnectionDialog;
        InfinoteManager *infinoteManager;
        Ui::ConnectionManager ui;
    
}; // class ConnectionManager

} // namespace Kobby

#endif
