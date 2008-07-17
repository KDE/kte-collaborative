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

namespace Kobby
{

class SessionManager
    : public KDialog
{
    
    public:
        SessionManager( QWidget *parent = 0 );
    
    private:
        Ui::SessionManager ui;
    
}; // class SessionManager

} // namespace Kobby

#endif
