#ifndef EMAILTEMPLATEDIALOG_H
#define EMAILTEMPLATEDIALOG_H

#include <QDialog>
#include <QString>
#include <QDebug>
namespace Ui {
class EmailTemplateDialog;
}

class EmailTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmailTemplateDialog(QWidget *parent = nullptr);
    ~EmailTemplateDialog();
    QString body = "No email body";
    QString subject = "No email subject";
    QString to = "No receiver email address";
    QString from = "No sender email";

public slots:
    QString getTo();
    QString getSubject();
    QString getBody();
    QString getFrom();

private:
    Ui::EmailTemplateDialog *ui;


private slots:
    void emitTemplateSignal();


signals:
    void emailTemplateSignal(const QString &sig = "NA");

};

#endif // EMAILTEMPLATEDIALOG_H
