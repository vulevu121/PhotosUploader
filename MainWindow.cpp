#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("Pixyl", "PixylBooth");

    settingsDialog = new SettingsDialog(this);
    connect(settingsDialog, SIGNAL(settingsSaved()), this, SLOT(syncSettings()));

    queueHeader = QStringList({
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
    ui->queueTableView->resizeColumnsToContents();

    watchHeader = QStringList({
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

    ui->watchTableView->resizeColumnsToContents();
    connect(ui->addQueueButton, SIGNAL(clicked()), this, SLOT(addQueue()));
    connect(ui->removeQueueButton, SIGNAL(clicked()), this, SLOT(removeQueues()));
    connect(ui->clearQueueButton, SIGNAL(clicked()), this, SLOT(clearQueue()));

    connect(ui->actionSetting, SIGNAL(triggered()), this->settingsDialog, SLOT(show()));
    connect(ui->addFolderButton, SIGNAL(clicked()), this, SLOT(addFolder()));
    connect(ui->removeFolderButton, SIGNAL(clicked()), this, SLOT(removeFolders()));
    connect(ui->clearWatchlistButton, SIGNAL(clicked()), this, SLOT(clearWatchlist()));

    connect(ui->actionCreateAlbum, SIGNAL(triggered()), this, SLOT(showCreateAlbumDialog()));
    connect(ui->actionResume,SIGNAL(triggered()),this,SLOT(queueTimerStart()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(queueTimerStop()));
    connect(ui->actionResume,SIGNAL(triggered()),this,SLOT(folderTimerStart()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(folderTimerStop()));
}

void MainWindow::syncSettings() {
    qDebug() << settings->value("scanningInterval", "10").toString();
}

void MainWindow::addQueue() {
    QFileDialog *fileDialog = new QFileDialog(this);

    QString filePath = fileDialog->getOpenFileName(this, tr("Select folder"), "", tr("Images (*.png *.jpg)"));
    QFileInfo fileInfo(filePath);

    if (filePath.length() > 0) {

        QList<QStandardItem *> queueRow({
            new QStandardItem(fileInfo.fileName()),
            new QStandardItem("MyAlbum"),
            new QStandardItem("Queue"),
            new QStandardItem(QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm AP")),
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
    QFileDialog *fileDialog = new QFileDialog(this);

    QString folderPath = fileDialog->getExistingDirectory(this, tr("Select folder"), "");
    QDir dir(folderPath);
    int num_files = dir.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files).length();

    QFileInfo fileInfo(folderPath);
//    qDebug() << dir.count();

    if (folderPath.length() > 0) {
        QList<QStandardItem *> watchRow({
            new QStandardItem(dir.dirName()),
            new QStandardItem("Queue"),
//            new QStandardItem(QString::number(dir.count())),
            new QStandardItem(QString::number(num_files)),
            new QStandardItem(QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm AP")),
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
    gphoto = new GooglePhoto(this);
    gphoto->SetAlbumName(name);
    gphoto->SetAlbumDescription(desc);
    connect(gphoto,SIGNAL(authenticated()),gphoto,SLOT(CreateAlbum()));
    connect(gphoto,SIGNAL(albumCreated()),gphoto,SLOT(ShareAlbum()));
    qDebug() << name << desc;

    queueTimerInit();
    queueTimerStart();

    folderTimerInit();
    folderTimerStart();

    /* Test saving log */
    QTimer::singleShot(60000,this,SLOT(saveLog()));

}

void MainWindow::showCreateAlbumDialog() {
    CreateAlbumDialog *dialog = new CreateAlbumDialog(this);
    dialog->show();
    connect(dialog, SIGNAL(createAlbumSignal(QString const &, QString const &)), this, SLOT(createAlbum(QString const &, QString const &)));
}


void MainWindow::queueTimerStart(){
    queueTimer->start(2000);
}

void MainWindow::queueTimerStop(){
    queueTimer->stop();
}

void MainWindow::queueTimerInit(){
    queueTimer = new QTimer(this);
    connect(queueTimer,SIGNAL(timeout()),this,SLOT(queueUpload()));
}

void MainWindow::folderTimerStart(){
    folderTimer->start(8000);
}

void MainWindow::folderTimerStop(){
    folderTimer->stop();
}

void MainWindow::folderTimerInit(){
    folderTimer = new QTimer(this);
    connect(folderTimer,SIGNAL(timeout()),this,SLOT(folderScan()));
}


void MainWindow::queueUpload(){
//     qDebug() << "Checking upload queue...";
     /* Iterate through the rows in the model, if status is Queue, upload photo,
      * If status is Completed, pass*/
     for(int row = 0; row < queueModel->rowCount();row++){
         if(queueModel->item(row,(queueHeader.indexOf("Status")))->text() == "Queue" && gphoto->isAlbumReady() && !gphoto->isUploading()){
            QString file = queueModel->item(row,(queueHeader.indexOf("Path")))->text();
            if(!uploadedList.contains(file)){
            qDebug() << "Uploading" << file;
            gphoto->UploadPhoto(file);

            /* Update queue model */
            queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Completed");
            queueModel->item(row,(queueHeader.indexOf("Album")))->setText(gphoto->GetAlbumName());

            connect(gphoto,SIGNAL(mediaCreated(QString)),this,SLOT(updateUploadedList(QString)));
            }
          }else{
//             qDebug() << "staus is not Queue for upload";
        }
     }
}

void MainWindow::updateUploadedList(QString filename){
    QFileInfo info(filename);
    QJsonObject obj;
    obj["name"] = info.fileName();
    obj["path"] = info.filePath();
    obj["album_name"] = gphoto->GetAlbumName();
    obj["album_url"] = gphoto->GetAlbumURL();
    obj["status"] = "Completed";
    obj["date_added"] = QDateTime::currentDateTime().toString("MM/dd/yyyy hh:mm AP");
    obj["url"] = gphoto->GetUploadedPhotoURL();

    uploadedListJson.append(obj);
    uploadedList.append(filename);
//    qDebug() << "After Uploaded list:" << uploadedList;
//    qDebug() << "After Uploaded list:" << uploadedListJson;

    isReady = true;
}

void MainWindow::folderScan(){
//     qDebug() << "Scanning watched folder...";
    /* make a list of the files in the queue */
    QStringList queue;
    for(int row = 0; row < queueModel->rowCount();row++){
        queue.append(queueModel->item(row,(queueHeader.indexOf("Path")))->text());
    }
    /* iterate the files in the folder */
    for(int row = 0; row < watchModel->rowCount();row++){
         if(watchModel->item(row,(watchHeader.indexOf("Status")))->text() == "Queue" ){
            QDir dir(watchModel->item(row,(watchHeader.indexOf("Path")))->text());
            qDebug() << "Scanning foldler" << dir.path();
            /* for each file in folder, if not in queue  and uploadedList, add to queue */
            QFileInfoList images = dir.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
            foreach(QFileInfo i, images){
                if (images.length() > 0 ) {
                    if( !queue.contains(i.filePath()) && isReady && !uploadedList.contains(i.filePath())){
                        QList<QStandardItem *> queueRow({
                            new QStandardItem(i.fileName()),
                            new QStandardItem("None"),
                            new QStandardItem("Queue"),
                            new QStandardItem(i.birthTime().toString()),
                            new QStandardItem(i.lastModified().toString()),
                            new QStandardItem(i.filePath())
                        });

                        this->queueModel->appendRow(queueRow);
                        ui->statusBar->showMessage("Queue added");
                        ui->queueTableView->resizeColumnsToContents();
                    }
                }
            }
            watchModel->item(row,(watchHeader.indexOf("No. Files")))->setText(QString::number(images.length()));

//            watchModel->item(row,(watchHeader.indexOf("Status")))->setText("Scanned");
            connect(gphoto,SIGNAL(mediaCreated(QString)),this,SLOT(updateUploadedList(QString)));
            }
     }
}


void MainWindow::saveLog(){
    qDebug() << "Saving log";
//    QString dir_path = camera_folder_path + "/";
    QFile jsonFile("C:/Users/khuon/Documents/Github/PhotosUploader/Upload Log.json");
    /* if log file does not exist, create a new one. Otherwise, overwrite */
    if (jsonFile.open(QIODevice::WriteOnly)) {
            QJsonDocument json_doc(uploadedListJson);
            QString json_string = json_doc.toJson();

            jsonFile.write(json_string.toLocal8Bit());
            jsonFile.close();
        }
        else{
            qDebug() << "failed to open save file" << endl;
            return;
        }
}

void MainWindow::emailLink(QString const &to, QString const &subject, QString const &body){
     email = new GMAIL();
     email->SetToEmail(to);
     email->SetFromEmail("khuongnguyensac@gmail.com");
     email->SetSubject(subject);
     email->SetBody(body);
     email->SetAlbumURL(gphoto->GetAlbumURL());
     connect(email,SIGNAL(linkReady()),email,SLOT(SendEmail()));

}


MainWindow::~MainWindow()
{
    delete ui;
}

