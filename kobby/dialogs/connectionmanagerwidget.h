#ifndef KOBBY_CONNECTIONMANAGERWIDGET_H
#define KOBBY_CONNECTIONMANAGERWIDGET_H

#include <QWidget>

class KPushButton;

namespace QInfinity
{
    class ConnectionListWidget;
}

namespace Kobby
{

class ConnectionManagerWidget
    : public QWidget
{
    Q_OBJECT
    
    public:
        ConnectionManagerWidget( QWidget *parent = 0 );

    private Q_SLOTS:
        void slotCreateConnection();
        void slotRemoveConnection();    

    private:
        void setupUi();
        void setupActions();
        
        QInfinity::ConnectionListWidget *connectionListWidget;
        KPushButton *addConnectionButton;
        KPushButton *removeConnectionButton;

};

}

#endif
