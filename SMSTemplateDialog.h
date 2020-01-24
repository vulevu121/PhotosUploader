#ifndef SMSTEMPLATEDIALOG_H
#define SMSTEMPLATEDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace Ui {
class SMSTemplateDialog;
}

class SMSTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SMSTemplateDialog(QWidget *parent = nullptr);
    ~SMSTemplateDialog();
    QString body = "<p><h3>No message body</h3></p>";
    QString from = "noreply";

private:
    Ui::SMSTemplateDialog *ui;
    QSettings *settings = new QSettings("Pixyl", "PixylPush");

private slots:
    void emitTemplateSignals();
    void loadSMSSettings();

public slots:
    QString getBody();
    QString getFrom();

signals:
    void smsTemplateSignal(const QString &sig = "NA");

};




#endif // SMSTEMPLATEDIALOG_H
