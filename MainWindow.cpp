#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settingsDialog = new SettingsDialog();

    QStringList queueHeader({
        "Filename",
        "Album",
        "Status",
        "Date Added",
        "Date Modified",
        "Path",
    });
    queueModel = new QStandardItemModel();
    queueModel->setHorizontalHeaderLabels(queueHeader);

    ui->queueTableView->setModel(queueModel);

//    QList<QStandardItem *> queueRow({
//        new QStandardItem("photo.jpg"),
//        new QStandardItem("MyAlbum"),
//        new QStandardItem("Queue"),
//        new QStandardItem("7/29/2019 8:32PM"),
//        new QStandardItem("/home/pictures/photo.jpg")
//    });

//    queueModel->appendRow(queueRow);

    ui->queueTableView->resizeColumnsToContents();

    QStringList watchHeader({
        "Folder",
        "Status",
        "No. Files",
        "Last Scanned",
        "Last Modified",
        "Path",
    });
    watchModel = new QStandardItemModel();
    watchModel->setHorizontalHeaderLabels(watchHeader);

    ui->watchTableView->setModel(watchModel);

//    QList<QStandardItem *> watchRow({
//        new QStandardItem("MyPictures"),
//        new QStandardItem("Scanned"),
//        new QStandardItem("1"),
//        new QStandardItem("7/29/2019 8:32PM"),
//        new QStandardItem("7/29/2019 8:32PM"),
//        new QStandardItem("/home/My Pictures")
//    });

//    watchModel->appendRow(watchRow);


    ui->watchTableView->resizeColumnsToContents();
    connect(ui->addQueueButton, SIGNAL(clicked()), this, SLOT(addQueue()));
    connect(ui->removeQueueButton, SIGNAL(clicked()), this, SLOT(removeQueues()));
    connect(ui->clearQueueButton, SIGNAL(clicked()), this, SLOT(clearQueue()));

    connect(ui->actionSetting, SIGNAL(triggered()), this->settingsDialog, SLOT(show()));
    connect(ui->addFolderButton, SIGNAL(clicked()), this, SLOT(addFolder()));
    connect(ui->removeFolderButton, SIGNAL(clicked()), this, SLOT(removeFolders()));
    connect(ui->clearWatchlistButton, SIGNAL(clicked()), this, SLOT(clearWatchlist()));

    connect(ui->actionCreateAlbum, SIGNAL(triggered()), this, SLOT(showCreateAlbumDialog()));

}

void MainWindow::addQueue() {
    QFileDialog *fileDialog = new QFileDialog();

    QString filePath = fileDialog->getOpenFileName(this, tr("Select folder"), "", tr("Images (*.png *.jpg)"));
    QFileInfo fileInfo(filePath);

    if (filePath.length() > 0) {

        QList<QStandardItem *> queueRow({
            new QStandardItem(fileInfo.fileName()),
            new QStandardItem("MyAlbum"),
            new QStandardItem("Queue"),
            new QStandardItem("7/29/2019 8:32PM"),
            new QStandardItem(fileInfo.lastModified().toString()),
            new QStandardItem(filePath)
        });

        this->queueModel->appendRow(queueRow);
        ui->statusBar->showMessage("Queue added");
        ui->queueTableView->resizeColumnsToContents();
    }

}

void MainWindow::removeQueues() {
    QModelIndexList selectedRows = ui->queueTableView->selectionModel()->selectedRows();

    foreach (QModelIndex index, selectedRows) {
        this->queueModel->removeRow(index.row());
    }

    ui->statusBar->showMessage("Queue removed");
}

void MainWindow::clearQueue() {
    this->queueModel->removeRows(0, this->queueModel->rowCount());

    ui->statusBar->showMessage("Queue cleared");
}



void MainWindow::addFolder() {
    QFileDialog *fileDialog = new QFileDialog();

    QString folderPath = fileDialog->getExistingDirectory(this, tr("Select folder"), "");
    QDir dir(folderPath);
    QFileInfo fileInfo(folderPath);
    qDebug() << dir.count();

    if (folderPath.length() > 0) {
        QList<QStandardItem *> watchRow({
            new QStandardItem(dir.dirName()),
            new QStandardItem("Scanned"),
            new QStandardItem(QString::number(dir.count())),
            new QStandardItem("7/29/2019 8:32PM"),
            new QStandardItem(fileInfo.lastModified().toString()),
            new QStandardItem(folderPath),
        });

        this->watchModel->appendRow(watchRow);

        ui->statusBar->showMessage("Folder added to watchlist");
        ui->watchTableView->resizeColumnsToContents();
    }


}

void MainWindow::removeFolders() {
    QModelIndexList selectedRows = ui->watchTableView->selectionModel()->selectedRows();

    foreach (QModelIndex index, selectedRows) {
        this->watchModel->removeRow(index.row());
    }

    ui->statusBar->showMessage("Folder(s) removed from watchlist");
}

void MainWindow::clearWatchlist() {
    this->watchModel->removeRows(0, this->watchModel->rowCount());

    ui->statusBar->showMessage("Watchlist cleared");
}

void MainWindow::createAlbum(QString const &name, QString const &desc) {
    qDebug() << name << desc;
}

void MainWindow::showCreateAlbumDialog() {
    CreateAlbumDialog *dialog = new CreateAlbumDialog();
    dialog->show();
    connect(dialog, SIGNAL(createAlbumSignal(QString const &, QString const &)), this, SLOT(createAlbum(QString const &, QString const &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

