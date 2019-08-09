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
    settings->setValue("onErrorAttemptInterval", ui->onErrorAttemptInterval->value());
    settings->setValue("playChimeUploadFinish",ui->playChimeUploadFinish->isChecked());
    settings->setValue("saveQueueExit",ui->saveQueueExit->isChecked());
    settings->setValue("showPreviewUpload",ui->showPreviewUpload->isChecked());
    settings->setValue("startMinimizedInTray",ui->startMinimizedInTray->isChecked());
    settings->setValue("startScanningStartup",ui->startScanningStartup->isChecked());

    settings->sync();
    emit settingsSaved();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
