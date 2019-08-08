#ifndef EMAILTEMPLATEDIALOG_H
#define EMAILTEMPLATEDIALOG_H

#include <QDialog>

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
};

#endif // EMAILTEMPLATEDIALOG_H
