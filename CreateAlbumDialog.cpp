#include "CreateAlbumDialog.h"
#include "ui_CreateAlbumDialog.h"

CreateAlbumDialog::CreateAlbumDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAlbumDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(emitCreateAlbumSignal()));
    connect(ui->newAlbumGroupbox, SIGNAL(toggled(bool)), this, SLOT(toggleExistingAlbumOption(bool)));
    connect(ui->existingAlbumGroupbox, SIGNAL(toggled(bool)), this, SLOT(toggleNewAlbumOption(bool)));

    useExistingAlbum = ui->existingAlbumGroupbox->isChecked();
//    qDebug() << newExistingAlbum;
}

void CreateAlbumDialog::emitCreateAlbumSignal() {

    emit createAlbumSignal(
                ui->albumNameEdit->text(),
                ui->albumDescEdit->text(),
                ui->existingAlbumIdEdit->text(),
                useExistingAlbum
                );
}

void CreateAlbumDialog::toggleExistingAlbumOption(bool on) {
    ui->existingAlbumGroupbox->setChecked(!on);
    useExistingAlbum = ui->existingAlbumGroupbox->isChecked();
//    qDebug() << newExistingAlbum;
}

void CreateAlbumDialog::toggleNewAlbumOption(bool on) {
    ui->newAlbumGroupbox->setChecked(!on);
//    newExistingAlbum = ui->newAlbumGroupbox->isChecked();
//    qDebug() << newExistingAlbum;
}

CreateAlbumDialog::~CreateAlbumDialog()
{
    delete ui;
}
