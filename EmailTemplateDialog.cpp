#include "EmailTemplateDialog.h"
#include "ui_EmailTemplateDialog.h"

EmailTemplateDialog::EmailTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmailTemplateDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(emitTemplateSignal()));

}

void EmailTemplateDialog::emitTemplateSignal(){
    to = ui->toEdit->text();
    subject = ui->subjectEdit->text();
    body = ui->bodyEdit->toPlainText();
    from = ui->fromEdit->text();
//    qDebug() << body;
    emit emailTemplateSignal();
}

QString EmailTemplateDialog::getTo(){
    return to;
}

QString EmailTemplateDialog::getFrom(){
    return from;
}
QString EmailTemplateDialog::getSubject(){
    return subject;

}QString EmailTemplateDialog::getBody(){
    return body;
}

EmailTemplateDialog::~EmailTemplateDialog()
{
    delete ui;
}
