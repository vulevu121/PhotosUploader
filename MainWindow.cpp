#include <QStandardItemModel>
#include <QStandardItem>

#include <QDebug>
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
        "Path",
    });
    queueModel = new QStandardItemModel();
    queueModel->setHorizontalHeaderLabels(queueHeader);

    ui->queueTableView->setModel(queueModel);

    QList<QStandardItem *> queueRow({
        new QStandardItem("photo.jpg"),
        new QStandardItem("MyAlbum"),
        new QStandardItem("Queue"),
        new QStandardItem("7/29/2019 8:32PM"),
        new QStandardItem("/home/pictures/photo.jpg")
    });

    queueModel->appendRow(queueRow);

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

    QList<QStandardItem *> watchRow({
        new QStandardItem("MyPictures"),
        new QStandardItem("Scanned"),
        new QStandardItem("1"),
        new QStandardItem("7/29/2019 8:32PM"),
        new QStandardItem("7/29/2019 8:32PM"),
        new QStandardItem("/home/My Pictures")
    });

    watchModel->appendRow(watchRow);


    ui->watchTableView->resizeColumnsToContents();
    connect(ui->addQueueButton, SIGNAL(clicked()), this, SLOT(addQueue()));
    connect(ui->removeQueueButton, SIGNAL(clicked()), this, SLOT(removeQueues()));
    connect(ui->clearQueueButton, SIGNAL(clicked()), this, SLOT(clearQueue()));

    connect(ui->actionSetting, SIGNAL(triggered()), this->settingsDialog, SLOT(show()));
    connect(ui->addFolderButton, SIGNAL(clicked()), this, SLOT(addFolder()));
    connect(ui->removeFolderButton, SIGNAL(clicked()), this, SLOT(removeFolders()));
    connect(ui->clearWatchlistButton, SIGNAL(clicked()), this, SLOT(clearWatchlist()));

}

void MainWindow::addQueue() {
    QList<QStandardItem *> queueRow({
        new QStandardItem("photo.jpg"),
        new QStandardItem("MyAlbum"),
        new QStandardItem("Queue"),
        new QStandardItem("7/29/2019 8:32PM"),
        new QStandardItem("/home/pictures/photo.jpg")
    });

    this->queueModel->appendRow(queueRow);
}

void MainWindow::removeQueues() {
    QModelIndexList selectedRows = ui->queueTableView->selectionModel()->selectedRows();

    foreach (QModelIndex index, selectedRows) {
        this->queueModel->removeRow(index.row());
    }
}

void MainWindow::clearQueue() {
    this->queueModel->removeRows(0, this->queueModel->rowCount());
}



void MainWindow::addFolder() {
    QList<QStandardItem *> watchRow({
        new QStandardItem("MyPictures"),
        new QStandardItem("Scanned"),
        new QStandardItem("1"),
        new QStandardItem("7/29/2019 8:32PM"),
        new QStandardItem("7/29/2019 8:32PM"),
        new QStandardItem("/home/My Pictures")
    });

    this->watchModel->appendRow(watchRow);
}

void MainWindow::removeFolders() {
    QModelIndexList selectedRows = ui->watchTableView->selectionModel()->selectedRows();

    foreach (QModelIndex index, selectedRows) {
        this->watchModel->removeRow(index.row());
    }
}

void MainWindow::clearWatchlist() {
    this->watchModel->removeRows(0, this->watchModel->rowCount());
}

MainWindow::~MainWindow()
{
    delete ui;
}

