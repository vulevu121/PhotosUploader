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
    from = ui->fromLineEdit->text();
    /* save sms settings to registry */
    qDebug() << "Saving SMS settings to registry";
    QJsonObject obj;
    obj["Body"] = body;
    obj["From"] = from;
    QJsonDocument doc(obj);
    settings->setValue("savedSMSSettings",QString(doc.toJson()));
    settings->sync();

    emit smsTemplateSignal();
}


void SMSTemplateDialog::loadSMSSettings(){
    QJsonDocument doc = QJsonDocument().fromJson(settings->value("savedSMSSettings").toByteArray());
    QJsonObject obj = doc.object();
    body = obj["Body"].toString();
    from = obj["From"].toString();
    ui->bodyEdit->insertPlainText(body);
    ui->fromLineEdit->setText(from);
}

QString SMSTemplateDialog::getBody(){
    return body;
}

QString SMSTemplateDialog::getFrom(){
    return from;
}
SMSTemplateDialog::~SMSTemplateDialog()
{
    delete ui;
}
