#ifndef SMSTEMPLATEDIALOG_H
#define SMSTEMPLATEDIALOG_H

#include <QDialog>

namespace Ui {
class SMSTemplateDialog;
}

class SMSTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SMSTemplateDialog(QWidget *parent = nullptr);
    ~SMSTemplateDialog();
    QString body;

private:
    Ui::SMSTemplateDialog *ui;

private slots:
    void emitTemplateSignals();

public slots:
    QString getBody();

signals:
    void smsTemplateSignal(const QString &sig = "NA");

};




#endif // SMSTEMPLATEDIALOG_H
