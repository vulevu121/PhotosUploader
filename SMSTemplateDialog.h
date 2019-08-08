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

private:
    Ui::SMSTemplateDialog *ui;
};

#endif // SMSTEMPLATEDIALOG_H
