#ifndef KOBBY_CREATEITEMDIALOG_H
#define KOBBY_CREATEITEMDIALOG_H

#include <KDialog>

#include <QString>

class QWidget;
class QString;

namespace Ui
{
    class CreateItemWidget;
}

namespace QInfinity
{
    class BrowserFolderItem;
}

namespace Kobby
{

class CreateItemDialog
    : public KDialog
{
    Q_OBJECT

    public:
        CreateItemDialog( QInfinity::BrowserFolderItem &parentItem,
            QWidget *parent = 0 );
        CreateItemDialog( QInfinity::BrowserFolderItem &parentItem,
            const QString &label,
            QWidget *parent = 0 );
        CreateItemDialog( QInfinity::BrowserFolderItem &parentItem,
            const QString &title,
            const QString &label,
            QWidget *parent = 0 );

    Q_SIGNALS:
        void create( QInfinity::BrowserFolderItem &parent,
            QString name );

    private Q_SLOTS:
        void slotOkClicked();

    private:
        void setupUi();
        void setupActions();

        Ui::CreateItemWidget *ui;
        QInfinity::BrowserFolderItem *m_parentItem;

};

}

#endif

