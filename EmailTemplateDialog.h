#ifndef EMAILTEMPLATEDIALOG_H
#define EMAILTEMPLATEDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class EmailTemplateDialog;
}

class EmailTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmailTemplateDialog(QWidget *parent = nullptr);
    ~EmailTemplateDialog();

private:
    Ui::EmailTemplateDialog *ui;

private slots:
    void emitSendEmailSignal();


signals:
    void sendEmailSignal(QString const &to, QString const &subject, QString const &body);

};

#endif // EMAILTEMPLATEDIALOG_H
