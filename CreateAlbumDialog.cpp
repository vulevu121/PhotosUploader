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

    newExistingAlbum = ui->existingAlbumGroupbox->isChecked();
//    qDebug() << newExistingAlbum;
}

void CreateAlbumDialog::emitCreateAlbumSignal() {
////    qDebug() << "emit create album signal";
//    if(ui->existingAlbumGroupbox->isChecked()){
////       emit existingAlbumSignal(ui)

//    }else{

//    }
    emit createAlbumSignal(
                ui->albumNameEdit->text(),
                ui->albumDescEdit->text(),
                ui->existingAlbumIdEdit->text(),
                newExistingAlbum
                );
}

void CreateAlbumDialog::toggleExistingAlbumOption(bool on) {
    ui->existingAlbumGroupbox->setChecked(!on);
    newExistingAlbum = ui->existingAlbumGroupbox->isChecked();
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
