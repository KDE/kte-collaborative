#ifndef KOBBY_JOINSESSIONDIALOG_H
#define KOBBY_JOINSESSIONDIALOG_H

#include <KDialog>

#include <QWidget>

#include "ui_joinsessiondialog.h"

namespace Kobby
{

class JoinSessionDialog
    : public KDialog
{
    
    Q_OBJECT
    
    public:
        JoinSessionDialog( QWidget *parent = 0 );
    
    private Q_SLOTS:
        void slotLocationChanged( const QString &text );
        void tryConnecting();
    
    private:
        void setupActions();
        
        Ui::JoinSessionDialog ui;
    
}; // class JoinSessionDialog

} // namespace Kobby

#endif
