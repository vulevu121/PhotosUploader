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
}

void CreateAlbumDialog::emitCreateAlbumSignal() {
//    qDebug() << "emit create album signal";
    emit createAlbumSignal(ui->albumNameEdit->text(), ui->albumDescEdit->text());
}

void CreateAlbumDialog::toggleExistingAlbumOption(bool on) {
    ui->existingAlbumGroupbox->setChecked(!on);
}

void CreateAlbumDialog::toggleNewAlbumOption(bool on) {
    ui->newAlbumGroupbox->setChecked(!on);
}

CreateAlbumDialog::~CreateAlbumDialog()
{
    delete ui;
}
