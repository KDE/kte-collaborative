#ifndef UPLOAD_DOCUMENT_DIALOG_H
#define UPLOAD_DOCUMENT_DIALOG_H

#include <KDialog>
#include <KLocale>

namespace Ui
{
    class SelectDestinationFolder;
};

namespace Kobby
{

class UploadDocumentDialog
    : public KDialog
{

    public:
        UploadDocumentDialog(const QString &doc_name = i18n("Untitled"),
            QWidget *parent = 0);

    private:
        Ui::SelectDestinationFolder *ui;

};

}

#endif

