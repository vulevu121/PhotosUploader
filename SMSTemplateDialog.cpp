#include "SMSTemplateDialog.h"
#include "ui_SMSTemplateDialog.h"

SMSTemplateDialog::SMSTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SMSTemplateDialog)
{
    ui->setupUi(this);
}

SMSTemplateDialog::~SMSTemplateDialog()
{
    delete ui;
}
