#include "SMSTemplateDialog.h"
#include "ui_SMSTemplateDialog.h"

SMSTemplateDialog::SMSTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SMSTemplateDialog)
{
    ui->setupUi(this);
    loadSMSSettings();
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(emitTemplateSignals()));

}

void SMSTemplateDialog::emitTemplateSignals(){
    body = ui->bodyEdit->toPlainText();

    /* save sms settings to registry */
    qDebug() << "Saving SMS settings to registry";
    QJsonObject obj;
    obj["Body"] = body;
    QJsonDocument doc(obj);
    settings->setValue("savedSMSSettings",QString(doc.toJson()));
    settings->sync();

    emit smsTemplateSignal();
}


void SMSTemplateDialog::loadSMSSettings(){
    QJsonDocument doc = QJsonDocument().fromJson(settings->value("savedSMSSettings").toByteArray());
    QJsonObject obj = doc.object();
    ui->bodyEdit->insertPlainText(obj["Body"].toString());
}

QString SMSTemplateDialog::getBody(){
    return body;
}

SMSTemplateDialog::~SMSTemplateDialog()
{
    delete ui;
}
