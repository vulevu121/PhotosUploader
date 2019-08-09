#include "EmailTemplateDialog.h"
#include "ui_EmailTemplateDialog.h"

EmailTemplateDialog::EmailTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmailTemplateDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(emitSendEmailSignal()));

}

void EmailTemplateDialog::emitSendEmailSignal(){
    emit sendEmailSignal(ui->toEdit->text(), ui->subjectEdit->text(), ui->bodyEdit->toPlainText());
}

EmailTemplateDialog::~EmailTemplateDialog()
{
    delete ui;
}
