#include "EmailTemplateDialog.h"
#include "ui_EmailTemplateDialog.h"

EmailTemplateDialog::EmailTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmailTemplateDialog)
{
    ui->setupUi(this);
    loadEmailSettings();
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(emitTemplateSignal()));

}

void EmailTemplateDialog::emitTemplateSignal(){
    to = ui->toEdit->text();
    subject = ui->subjectEdit->text();
    body = ui->bodyEdit->toPlainText();
    from = ui->fromEdit->text();

    /* save email settings to registry */
    qDebug() << "Saving email settings to registry";
    QJsonObject obj;
    obj["To"] = to ;
    obj["From"] = from;
    obj["Subject"] = subject ;
    obj["Body"] = body;
    QJsonDocument doc(obj);
    settings->setValue("savedEmailSettings",QString(doc.toJson()));
    settings->sync();

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

void EmailTemplateDialog::loadEmailSettings(){
    QJsonDocument doc = QJsonDocument().fromJson(settings->value("savedEmailSettings").toByteArray());
    QJsonObject obj = doc.object();
    to = obj["To"].toString();
    subject = obj["Subject"].toString();
    from = obj["From"].toString();
    body = obj["Body"].toString();
    ui->toEdit->setText(to);
    ui->subjectEdit->setText(subject);
    ui->fromEdit->setText(from);
    ui->bodyEdit->insertPlainText(body);
}

EmailTemplateDialog::~EmailTemplateDialog()
{
    delete ui;
}
