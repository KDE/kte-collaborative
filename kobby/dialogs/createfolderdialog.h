#ifndef KOBBY_CREATEFOLDERDIALOG_H
#define KOBBY_CREATEFOLDERDIALOG_H

#include <KDialog>

class QWidget;
class QString;

namespace Ui
{
    class CreateFolderWidget;
}

namespace Kobby
{

class CreateFolderDialog
    : public KDialog
{

    public:
        CreateFolderDialog( QWidget *parent = 0 );

        QString folderName() const;
    
    private:
        Ui::CreateFolderWidget *ui;

};

}

#endif

