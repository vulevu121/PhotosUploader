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
    connect(ui->actionResume,SIGNAL(triggered()),this,SLOT(resumeQueue()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(stopQueue()));

    /* Initialize the scan timers */
    queueTimerInit();
    folderTimerInit();

    /* Use for testing oauth2 only*/
//    gphoto = new GooglePhoto(this);
//    QTimer::singleShot(30000,gphoto,SLOT(Reauthenticate()));

    connect(ui->actionEmail, SIGNAL(triggered()), this, SLOT(showEmailTemplate()));
    connect(ui->actionSMS, SIGNAL(triggered()), this, SLOT(showSMSTemplate()));
}

void MainWindow::syncSettings() {
//    qDebug() << "scanning internal" <<settings->value("scanningInterval", "10").toInt(); //done
//    qDebug() << "on error retries" <<  settings->value("onErrorRetries","10").toInt();
//    qDebug() << "on error interval" << settings->value("onErrorAttemptInterval","10").toInt();
//    qDebug() << "play chime" << settings->value("playChimeUploadFinish").toBool();
//    qDebug() << "save queue exit"<< settings->value("saveQueueExit").toBool();
//    qDebug() << "show preview upload"<< settings->value("showPreviewUpload").toBool();
//    qDebug() << "start minimize"<< settings->value("startMinimizedInTray").toBool();
//    qDebug() << "start scan start up"<< settings->value("startScanningStartup").toBool(); //done


   /* If timer is not initilize yet, do nothing. The time will sync automatically by the Setting object
    * Otherwise, restart the active timer to update the interval
    * If setting is triggerd before timer is initilize, you will crash */
    if(queueTimer != nullptr && folderTimer != nullptr){
        if(settings->value("scanningInterval").toInt() != queueTimer->interval()){
            qDebug() << "Scan interval changed.";
            stopQueue();
            resumeQueue();
        }else{
            qDebug() << "Scan interval NOT changed.";
        }
    }else{
        qDebug() << "Unable to change scan interval. Timer is not initialized.";
    }

    /* Start scan on start up option */
    if(settings->value("startScanningStartup").toBool()){resumeQueue();}

    /* Save queue on exit */
    if(settings->value("saveQueueExit").toBool()){
//        connect(ui->MainWindow,&QWidget::destroyed,this,MainWindow::saveLog()));
    }

}

void MainWindow::queueTimerStart(){
    qDebug() << "queue timer start";
//    queueTimer->start(2000);
    queueTimer->start(settings->value("scanningInterval", "10").toInt() * 1000);

}

void MainWindow::queueTimerStop(){
    qDebug() << "queue timer stop";
    queueTimer->stop();
}

void MainWindow::queueTimerInit(){
    queueTimer = new QTimer(this);
    connect(queueTimer,SIGNAL(timeout()),this,SLOT(queueUpload()));
}

void MainWindow::folderTimerStart(){
    qDebug() << "folder timer start";
//    folderTimer->start(8000);
    folderTimer->start(settings->value("scanningInterval", "10").toInt() * 1000); // convert to ms
}

void MainWindow::folderTimerStop(){
    qDebug() << "folder timer stop";
    folderTimer->stop();
}

void MainWindow::folderTimerInit(){
    folderTimer = new QTimer(this);
    connect(folderTimer,SIGNAL(timeout()),this,SLOT(folderScan()));
}

void MainWindow::resumeQueue(){
    if(queueTimer != nullptr && folderTimer != nullptr){
            queueTimerStart();
            folderTimerStart();

        }else{
            qDebug() << "Unable to resume scan. Timer is not initialized";
        }
}

void MainWindow::stopQueue(){
    if(queueTimer != nullptr && folderTimer != nullptr){
            queueTimerStop();
            folderTimerStop();
    }else{
        qDebug() << "Unable to stop scan. Timer is not initialized";
    }
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

void MainWindow::createAlbum(QString const &name, QString const &desc, QString const &albumId, bool useExistingAlbum) {
    gphoto = new GooglePhoto(this);
    if (useExistingAlbum) {
        gphoto->SetTargetAlbumToUpload(albumId);
        qDebug() << gphoto->GetAlbumID();

    } else {
        gphoto->SetAlbumName(name);
        gphoto->SetAlbumDescription(desc);
        connect(gphoto,SIGNAL(authenticated()),gphoto,SLOT(CreateAlbum()));
        connect(gphoto,SIGNAL(albumCreated()),gphoto,SLOT(ShareAlbum()));
    }

    /* Show message */
    connect(gphoto,SIGNAL(showMessage(QString const &)), ui->statusBar, SLOT(showMessage(QString const &)));

    /* Test saving log */
    QTimer::singleShot(120000,this,SLOT(saveLog()));

}

void MainWindow::showCreateAlbumDialog() {
    CreateAlbumDialog *dialog = new CreateAlbumDialog(this);
    dialog->show();
    connect(dialog, SIGNAL(createAlbumSignal(QString const, QString const , QString const , bool )), this, SLOT(createAlbum(QString const, QString const , QString const , bool )));
}



void MainWindow::queueUpload(){
//     qDebug() << "Checking upload queue...";
     /* Iterate through the rows in the model, if status is Queue, upload photo,
      * If status is Completed, pass*/
     for(int row = 0; row < queueModel->rowCount();row++){
         QString file = queueModel->item(row,(queueHeader.indexOf("Path")))->text();
         if(queueModel->item(row,(queueHeader.indexOf("Status")))->text() == "Queue"){
                 if( gphoto->isAlbumReady() && !gphoto->isUploading()){
                    if(!uploadedList.contains(file)){
                        qDebug() << "Uploading" << file;
                        gphoto->UploadPhoto(file);

                        /* Update queue model */
                        queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Uploading");
                        queueModel->item(row,(queueHeader.indexOf("Album")))->setText(gphoto->GetAlbumName());
                        connect(gphoto,SIGNAL(mediaCreated(QString)),this,SLOT(updateUploadedList(QString)));
                    }else{
                        qDebug() << "File is already uploaded";
                     }
                   }
         }else if(queueModel->item(row,(queueHeader.indexOf("Status")))->text() == "Uploading"){
                  /* if the status is "Uploading", check if it is complete, if so then change status to completed */
             if(uploadedList.contains(file)){queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Completed");}
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
    obj["album_id"] = gphoto->GetAlbumID();
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

//    QFile jsonFile("C:/Users/khuon/Documents/Github/PixylPush/Upload Log.json");
    QFile jsonFile("C:/");

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

void MainWindow::sendNow(QString const &to, QString const &subject, QString const &body){
     email = new GMAIL();
     email->SetToEmail(to);
     email->SetToEmail("7143529299@tmomail.net");
     email->SetFromEmail("khuongnguyensac@gmail.com");
     email->SetSubject(subject);
     email->SetBody(body);
     email->SetAlbumURL(gphoto->GetAlbumURL());
     connect(email,SIGNAL(authenticated()),email,SLOT(SendEmail()));
}

void MainWindow::showEmailTemplate() {
    EmailTemplateDialog *emailDialog = new EmailTemplateDialog(this);
    emailDialog->show();
    connect(emailDialog,SIGNAL(sendEmailSignal(QString const,QString const , QString const)),this,SLOT(sendNow(QString const,QString const , QString const)));
}

void MainWindow::showSMSTemplate() {
    SMSTemplateDialog *smsDialog = new SMSTemplateDialog(this);
    smsDialog->show();
}


MainWindow::~MainWindow()
{
    delete ui;
}

