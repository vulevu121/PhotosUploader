#include "EmailTemplateDialog.h"
#include "ui_EmailTemplateDialog.h"

EmailTemplateDialog::EmailTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmailTemplateDialog)
{
    ui->setupUi(this);
}

EmailTemplateDialog::~EmailTemplateDialog()
{
    delete ui;
}
