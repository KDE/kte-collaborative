#ifndef KOBBY_SESSIONMANAGER_H
#define KOBBY_SESSIONMANAGER_H

#include <KDialog>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "ui_sessionmanager.h"
#include "joinsessiondialog.h"

namespace Kobby
{

class SessionManager
    : public KDialog
{
    
    Q_OBJECT
    
    public:
        SessionManager( QWidget *parent = 0 );
        ~SessionManager();
    
    public Q_SLOTS:
        void slotJoinSession();
        void slotJoinSessionFinished();
    
    private:
        void setupActions();
        
        JoinSessionDialog *joinSessionDialog;
        Ui::SessionManager ui;
    
}; // class SessionManager

} // namespace Kobby

#endif
