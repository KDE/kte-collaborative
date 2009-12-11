/*
 * Copyright 2009  Gregory Haynes <greg@greghaynes.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "closedocsdialog.h"
#include "editor/documentmodel.h"
#include "editor/document.h"

#include "ui_closedocswidget.h"
#include "closedocsdialog.moc"

#include <KIcon>

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>

namespace Kobby
{

class CloseDocumentItem
    : public QListWidgetItem
{

    public:
        CloseDocumentItem( Document &doc );

        Document &document();

    private:
        Document *m_doc;

};

CloseDocumentItem::CloseDocumentItem( Document &doc )
    : QListWidgetItem ()
    , m_doc( &doc )
{
    setText( doc.name() );
}

Document &CloseDocumentItem::document()
{
    return *m_doc;
}

CloseDocsDialog::CloseDocsDialog(DocumentModel &docModel,
    QWidget *parent )
    : KDialog( parent )
    , ui( new Ui::CloseDocsWidget )
{
    QWidget *mw = new QWidget( this );
    Document *doc;
    QList<Document*> doclist;    
    CloseDocumentItem *docItem;

    setWindowTitle( i18n( "Confirm exit" ) );
    ui->setupUi( mw );
    
    doclist = docModel.dirtyDocs();
    foreach(doc, doclist)
    {
        docItem = new CloseDocumentItem( *doc );
        docItem->setCheckState( Qt::Checked );
        ui->localDocList->addItem( docItem );
    }

    doclist = docModel.collabDocs();
    foreach(doc, doclist)
    {
        docItem = new CloseDocumentItem( *doc );
        ui->collabDocList->addItem( docItem );
    }

    setButtons( User1 | User2 | User3 );
    setButtonText( User1, i18n( "Cancel Exit" ) );
    setButtonText( User2, i18n( "Dont Save" ) );
    setButtonText( User3, i18n( "Save" ) );

    setButtonIcon( User1, KIcon("dialog-cancel.png") );
    setButtonIcon( User3, KIcon("document-save.png") );

    connect( this, SIGNAL(user1Clicked()),
        this, SLOT(onCancel()) );
    connect( this, SIGNAL(user2Clicked()),
        this, SLOT(onDontSave()) );
    connect( this, SIGNAL(user3Clicked()),
        this, SLOT(onSave()) );
    
    setMainWidget( mw );
}

void CloseDocsDialog::onCancel()
{
    reject();
}

void CloseDocsDialog::onDontSave()
{
    accept();
}

void CloseDocsDialog::onSave()
{
    QListWidgetItem *stdItem;
    CloseDocumentItem *closeItem;
    int i;
    for(i = 0;(stdItem = ui->localDocList->item(i));i++)
    {
        closeItem = dynamic_cast<CloseDocumentItem*>(stdItem);
        if(closeItem)
        {
            if(!closeItem->document().save())
            {
                reject();
                return;
            }
        }
    }
    accept();
}

}

