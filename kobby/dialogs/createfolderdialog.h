#ifndef KOBBY_CREATEFOLDERDIALOG_H
#define KOBBY_CREATEFOLDERDIALOG_H

#include <KDialog>

class QWidget;
class QString;

namespace Ui
{
    class CreateFolderWidget;
}

namespace QInfinity
{
    class BrowserFolderItem;
}

namespace Kobby
{

class CreateFolderDialog
    : public KDialog
{
    Q_OBJECT

    public:
        CreateFolderDialog( const QInfinity::BrowserFolderItem &parentItem,
            QWidget *parent = 0 );

        QString folderName() const;
    
    Q_SIGNALS:
        void createFolder( const QInfinity::BrowserFolderItem &parent,
            QString name );

    private Q_SLOTS:
        void slotOkClicked();

    private:
        const QInfinity::BrowserFolderItem *m_parent;
        Ui::CreateFolderWidget *ui;

};

}

#endif

