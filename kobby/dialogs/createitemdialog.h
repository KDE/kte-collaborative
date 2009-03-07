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

/**
 * @brief Generic dialog for creating a named item.
 */
class CreateItemDialog
    : public KDialog
{

    public:
        CreateItemDialog( QString title,
            QString label,
            QWidget *parent = 0 );

        QString name() const;

    private:
        Ui::CreateItemWidget *ui;

};

}

#endif

