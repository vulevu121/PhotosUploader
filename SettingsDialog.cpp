#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));

    ui->onErrorRetries->setValue(settings->value("onErrorRetries", "10").toInt());
    ui->scanningInterval->setValue(settings->value("scanningInterval", "10").toInt());
}

void SettingsDialog::saveSettings() {
    settings->setValue("onErrorRetries", ui->onErrorRetries->value());
    settings->setValue("scanningInterval", ui->scanningInterval->value());
    settings->sync();
    emit settingsSaved();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
