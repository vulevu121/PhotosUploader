#include "SMSTemplateDialog.h"
#include "ui_SMSTemplateDialog.h"

SMSTemplateDialog::SMSTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SMSTemplateDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(emitTemplateSignals()));

}

void SMSTemplateDialog::emitTemplateSignals(){
    body = ui->bodyEdit->toPlainText();
    emit smsTemplateSignal();
}

QString SMSTemplateDialog::getBody(){
    return body;
}

SMSTemplateDialog::~SMSTemplateDialog()
{
    delete ui;
}
