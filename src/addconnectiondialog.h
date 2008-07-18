#ifndef KOBBY_ADDCONNECTIONDIALOG_H
#define KOBBY_ADDCONNECTIONDIALOG_H

#include <KDialog>

#include <QWidget>

#include "ui_addconnectiondialog.h"

namespace Kobby
{

class AddConnectionDialog
    : public KDialog
{
    
    Q_OBJECT
    
    public:
        AddConnectionDialog( QWidget *parent = 0 );
    
    private Q_SLOTS:
        void slotLocationChanged( const QString &text );
        void tryConnecting();
    
    private:
        void setupActions();
        
        Ui::AddConnectionDialog ui;
    
}; // class AddConnectionDialog

} // namespace Kobby

#endif
