#include <QStandardItemModel>
#include <QStandardItem>
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settingsDialog = new SettingsDialog();
    connect(ui->actionSetting, SIGNAL(triggered()), this->settingsDialog, SLOT(show()));

    QStringList queueHeader({
        "Filename",
        "Album",
        "Status",
        "Date Added",
        "Path",
    });
    QStandardItemModel *queueModel = new QStandardItemModel();
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
    QStandardItemModel *watchModel = new QStandardItemModel();
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


}

MainWindow::~MainWindow()
{
    delete ui;
}

