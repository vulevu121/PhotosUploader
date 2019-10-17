#include "CreateAlbumDialog.h"
#include "ui_CreateAlbumDialog.h"

CreateAlbumDialog::CreateAlbumDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAlbumDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(emitAlbumSignal()));
    connect(ui->newAlbumGroupbox, SIGNAL(toggled(bool)), this, SLOT(toggleExistingAlbumOption(bool)));
    connect(ui->existingAlbumGroupbox, SIGNAL(toggled(bool)), this, SLOT(toggleNewAlbumOption(bool)));

    ui->existingAlbumGroupbox->setChecked(true);
}

void CreateAlbumDialog::emitAlbumSignal() {
    if (ui->existingAlbumGroupbox->isChecked())
        emit existingAlbumSignal(lastAlbumId);
    else
        emit createAlbumSignal(
                    ui->albumNameEdit->text(),
                    ui->albumDescEdit->text()
                    );

}

void CreateAlbumDialog::toggleExistingAlbumOption(bool on) {
    ui->existingAlbumGroupbox->setChecked(!on);
//    bool useExistingAlbum = ui->existingAlbumGroupbox->isChecked();
//    qDebug() << "existing album:" << useExistingAlbum;

}

void CreateAlbumDialog::toggleNewAlbumOption(bool on) {
    ui->newAlbumGroupbox->setChecked(!on);
//    bool newAlbum = ui->newAlbumGroupbox->isChecked();
//    qDebug() << "new album:" << newAlbum;
}

void CreateAlbumDialog::setExistingAlbum(QString const &id, QString const &name){
    lastAlbumId = id;
    lastAlbumName = name;
    ui->existingAlbumNameEdit->setText(name);
}

CreateAlbumDialog::~CreateAlbumDialog()
{
    delete ui;
}
