#include "CreateAlbumDialog.h"
#include "ui_CreateAlbumDialog.h"

CreateAlbumDialog::CreateAlbumDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAlbumDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(emitCreateAlbumSignal()));
}

void CreateAlbumDialog::emitCreateAlbumSignal() {
//    qDebug() << "emit create album signal";
    emit createAlbumSignal(ui->albumNameEdit->text(), ui->albumDescEdit->text());
}


CreateAlbumDialog::~CreateAlbumDialog()
{
    delete ui;
}
