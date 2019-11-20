#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("Pixyl", "PixylPush");

    settingsDialog = new SettingsDialog(this);

    settings.sync();
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
    connect(ui->addFolderButton, SIGNAL(clicked()), this, SLOT(addFolders()));
    connect(ui->removeFolderButton, SIGNAL(clicked()), this, SLOT(removeFolders()));
    connect(ui->clearWatchlistButton, SIGNAL(clicked()), this, SLOT(clearWatchlist()));

    connect(ui->actionCreateAlbum, SIGNAL(triggered()), this, SLOT(showCreateAlbumDialog()));
    connect(ui->actionResume,SIGNAL(triggered()),this,SLOT(resumeQueue()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(stopQueue()));
    connect(ui->actionLogIn,SIGNAL(triggered()),this,SLOT(googleLogIn()));
    connect(ui->actionLogOut,SIGNAL(triggered()),this,SLOT(googleLogOut()));

    connect(ui->actionEmail, SIGNAL(triggered()), this, SLOT(showEmailTemplate()));
    connect(ui->actionSMS, SIGNAL(triggered()), this, SLOT(showSMSTemplate()));


    /* disable some options */
    disableLogOutBtn();
    disableCreateAlbumBtn();

    /* Start scan on start up option */
    if(settings.value("startScanningStartup").toBool())
        resumeQueue();


    /* Initialize the scan timers */
    queueTimerInit();
    folderTimerInit();
    saveTimerInit();

}

void MainWindow::disableCreateAlbumBtn(){
    ui->actionCreateAlbum->setEnabled(false);
    ui->actionCreateAlbum->setIcon(colorIcon(":/icon/photo_album", grey));
}

void MainWindow::enableCreateAlbumBtn(QString const &blank){
    ui->actionCreateAlbum->setEnabled(true);
    ui->actionCreateAlbum->setIcon(colorIcon(":/icon/photo_album", white));
}

void MainWindow::disableLogInBtn(){
    ui->actionLogIn->setEnabled(false);
    ui->actionLogIn->setIcon(colorIcon(":/icon/photo_album", grey));
}

void MainWindow::enableLogInBtn(QString const &blank){
    ui->actionLogIn->setEnabled(true);
    ui->actionLogIn->setIcon(colorIcon(":/icon/photo_album", white));
}

void MainWindow::disableLogOutBtn(){
    ui->actionLogOut->setEnabled(false);
    ui->actionLogOut->setIcon(colorIcon(":/icon/photo_album", grey));
}

void MainWindow::enableLogOutBtn(QString const &blank){
    ui->actionLogOut->setEnabled(true);
    ui->actionLogOut->setIcon(colorIcon(":/icon/photo_album", white));
}

void MainWindow::displayAlbumName(QString const &id, QString const &name){
    qDebug() << name << id;
    ui->albumNameButton->setText(name);
}

/************************** Log In / Log Out **************************/
void MainWindow::googleLogIn(){
    gphoto = new GooglePhoto(this);
    auth = new GoogleOAuth2(this);
    auth->SetScope();
    auth->Authenticate();
    connect(auth,SIGNAL(authenticated(QString const &)),gphoto,SLOT(SetAccessToken(QString const &)));
    connect(auth,SIGNAL(showMessage(QString const &)),ui->statusBar, SLOT(showMessage(QString const &)));

    /* enable album create button & disable login button */
    connect(auth,SIGNAL(authenticated(QString const)),this,SLOT(enableLogOutBtn()));
    connect(auth,SIGNAL(authenticated(QString const)),this,SLOT(disableLogInBtn()));
    connect(auth,SIGNAL(authenticated(QString const)),this,SLOT(enableCreateAlbumBtn()));

}

void MainWindow::googleLogOut(){
    if(auth != nullptr){
        ui->statusBar->showMessage("Logged out. Queue cleared. Watch folder removed.");
        stopQueue();
        clearQueue();
        clearWatchlist();

        /* disable album create button & enable login button*/
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(disableLogOutBtn()));
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(enableLogInBtn()));
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(disableCreateAlbumBtn()));

        auth->deleteCookies();
        auth->quit();
        deleteAllObjects();

    }else
        ui->statusBar->showMessage("You are not currently logged in.");
}

void MainWindow::deleteAllObjects(){
//    qDebug() << "Before delete:" << uploadedList  << uploadedListJson << uploadFailedList;
    gphoto = nullptr;
    auth = nullptr;
    uploadedList = QStringList();
    uploadedListJson = QJsonArray();
    uploadFailedList = QMap<QString,int>();
    isReady = true;
//    qDebug() << "After delete:" << uploadedList  << uploadedListJson << uploadFailedList;
}
/************************** END **************************************/



QIcon MainWindow::colorIcon(const QString &path, const QColor &color) {
    QPixmap pixmap = QPixmap(path);
    QBitmap mask = pixmap.createMaskFromColor(QColor("transparent"), Qt::MaskInColor);
    pixmap.fill(QColor(color));
    pixmap.setMask(mask);
    return QIcon(pixmap);
}


/************************** Settings **************************************/
void MainWindow::syncSettings() {
//    qDebug() << "scanning internal" <<settings->value("scanningInterval", "10").toInt(); //done
//    qDebug() << "on error retries" <<  settings->value("onErrorRetries","10").toInt();
//    qDebug() << "on error interval" << settings->value("onErrorAttemptInterval","10").toInt();
//    qDebug() << "play chime" << settings->value("playChimeUploadFinish").toBool();
//    qDebug() << "save queue exit"<< settings->value("saveQueueExit").toBool();
//    qDebug() << "show preview upload"<< settings->value("showPreviewUpload").toBool();
//    qDebug() << "start minimize"<< settings->value("startMinimizedInTray").toBool();


   /* If timer is not initilize yet, do nothing. The time will sync automatically by the Setting object
    * Otherwise, restart the active timer to update the interval
    * If setting is triggerd before timer is initilize, you will crash */
    if(queueTimer != nullptr && folderTimer != nullptr){
        if(settings->value("scanningInterval").toInt() != queueTimer->interval()){
            qDebug() << "Scan interval changed.";
            stopQueue();
            resumeQueue();
        }else
            qDebug() << "Scan interval NOT changed.";

    }else
        qDebug() << "Unable to change scan interval. Timer is not initialized.";


    /* Save queue on exit */
    if(settings->value("saveQueueExit").toBool()){
//        connect(ui->MainWindow,&QWidget::destroyed,this,MainWindow::saveProgress()));
    }

}
/************************** End **************************************/

/************************** Timer **************************************/
void MainWindow::saveTimerInit(){
    saveTimer = new QTimer(this);
    saveTimer->start(30000);
    connect(saveTimer,SIGNAL(timeout()),this,SLOT(saveProgress()));
}

void MainWindow::queueTimerStart(){
    qDebug() << "queue timer start";
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
        ui->statusBar->showMessage("Scanning resumed");
        ui->actionResume->setIcon(colorIcon(":/icon/resume", QColor(0, 255, 0)));
     }else
        qDebug() << "Unable to resume scan. Timer is not initialized";
}

void MainWindow::stopQueue(){
    if(queueTimer != nullptr && folderTimer != nullptr){
        queueTimerStop();
        folderTimerStop();
        ui->statusBar->showMessage("Scanning stopped");
        ui->actionResume->setIcon(colorIcon(":/icon/resume", QColor(255, 255, 255)));
    }else
        qDebug() << "Unable to stop scan. Timer is not initialized";
}

/************************** END **************************************/


/************************** Queue **************************************/


void MainWindow::addQueue() {
    /* make a list of the files in the queue */
    QStringList queue;
    for(int row = 0; row < queueModel->rowCount();row++)
        queue.append(queueModel->item(row,(queueHeader.indexOf("Path")))->text());

   /* add a file to queue */
    QFileDialog *fileDialog = new QFileDialog(this);
    QString filePath = fileDialog->getOpenFileName(this, tr("Select folder"), "", tr("Images (*.png *.jpg)"));
    QFileInfo fileInfo(filePath);

    if (filePath.length() > 0) {
        if(!queue.contains(filePath)){
            QStandardItem *item = new QStandardItem("some text");
            item->setIcon(colorIcon(":/icon/check", QColor(0, 255, 0)));

            QList<QStandardItem *> queueRow({
                new QStandardItem(fileInfo.fileName()),
                new QStandardItem("MyAlbum"),
                new QStandardItem("Queue"),
                new QStandardItem(QDateTime::currentDateTime().toString(timeFormat)),
                new QStandardItem(fileInfo.lastModified().toString(timeFormat)),
                new QStandardItem(filePath),
                item
            });

            this->queueModel->appendRow(queueRow);
            ui->statusBar->showMessage("Queue added");
            ui->queueTableView->resizeColumnsToContents();
        }else
            ui->statusBar->showMessage(filePath + QString(" is already added."));
    }
}

void MainWindow::removeQueues() {
    QModelIndexList selectedRows = ui->queueTableView->selectionModel()->selectedRows();
    foreach (QModelIndex index, selectedRows)
        this->queueModel->removeRow(index.row());

    ui->statusBar->showMessage("Queue removed");
}

void MainWindow::clearQueue() {
    this->queueModel->removeRows(0, this->queueModel->rowCount());
    ui->statusBar->showMessage("Queue cleared");
}

void MainWindow::queueUpload(){
//     qDebug() << "Checking upload queue...";
     /* Iterate through the rows in the model, if status is Queue, upload photo,
      * If status is Completed, pass*/
     QTime t;
     t.start();
     //qDebug() << t.msec();
     for(int row = 0; row < queueModel->rowCount();row++){
             QString file = queueModel->item(row,(queueHeader.indexOf("Path")))->text();

             if(queueModel->item(row,(queueHeader.indexOf("Status")))->text() == "Queue"){
//                 qDebug() << "+";
//                 qDebug() << gphoto->isAlbumReady() << !gphoto->isUploading();

                     if(gphoto->isAlbumReady() && !gphoto->isUploading()){
                            if(!uploadedList.contains(file) && isReady){
                                /* Using isReady flag to prevent duplicate entry being saved to
                                 * the json log file. Basically, each photo will be uploaded sequencially and
                                 * the uploadedList will always include all the uploaded photo. There wont be
                                 * an instance where this upload loop runs before the uploadedList function is done */
                                isReady = false;
                                qDebug() << "Uploading" << file;
                                gphoto->UploadPhoto(file);

                                /* Update queue model */
                                queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Uploading");
                                queueModel->item(row,(queueHeader.indexOf("Album")))->setText(gphoto->GetAlbumName());
                                connect(gphoto,SIGNAL(mediaCreated(QString const)),this,SLOT(updateUploadedList(QString const)));
                                connect(gphoto,SIGNAL(mediaCreateFailed(QString const)),this,SLOT(updateFailedList(QString const)));
                            }else{
                                /* In case internet connection broke and resume */
                                QString msg = file + "File is already uploaded";
                                ui->statusBar->showMessage(msg);
                                queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Completed");
                                queueModel->item(row,(queueHeader.indexOf("Album")))->setText(gphoto->GetAlbumName());
                            }
                       }
             }else if(queueModel->item(row,(queueHeader.indexOf("Status")))->text() == "Uploading"){
                      /* if the status is "Uploading", check if it is complete, if so then change status to completed */
//                     qDebug() << "-";

                     if(uploadedList.contains(file)){queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Completed");
                     }else if(uploadFailedList.contains(file)){
                                qDebug() << "--";

                                if( uploadFailedList.value(file) < settings->value("onErrorRetries","10").toInt())
                                        {queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Queue");
                                 }else{queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Failed");
                                    if(!elapsedTime.isActive()){
                                        /* Call function once. Avoid multile singleshot called*/
                                        elapsedTime.singleShot(settings->value("onErrorAttemptInterval","10").toInt(),this,SLOT(resetFailItems()));
                                        }
                                  }
                            }
                   }
         }     //qDebug() << t.msec();

}

void MainWindow::resetFailItems(){
    for(int row = 0; row < queueModel->rowCount();row++){
            if(queueModel->item(row,(queueHeader.indexOf("Status")))->text() == "Failed"){
                queueModel->item(row,(queueHeader.indexOf("Status")))->setText("Queue");
            }
    }
    elapsedTime.stop();
}

/************************** END **************************************/


/************************** Folder **************************************/
void MainWindow::addFolders(){
    QFileDialog *fileDialog = new QFileDialog(this);
    QString folderPath = fileDialog->getExistingDirectory(this, tr("Select Event Folder"), "");
    /* if "print" and "camera" folders do not exist in this directory, add the files
     * in here to the queue like normal. Else, scan the 2 folders automatically */

    /* create QDir for "camera" and "print" folders */
    QDir print (folderPath + "/print");
    QDir camera (folderPath + "/camera");

    if(print.exists() && camera.exists()){
        addFolder(print.path());
        addFolder(camera.path());
        qDebug() << print.path();
        qDebug() << camera.path();
    }else
        addFolder(folderPath);
}

void MainWindow::addFolder(QString const &folderPath) {
//    QFileDialog *fileDialog = new QFileDialog(this);
//    QString folderPath = fileDialog->getExistingDirectory(this, tr("Select folder"), "");
    QDir dir(folderPath);

    int num_files = dir.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files).length();
    /* Make a list of the folder paths in the watchModel  */
    QStringList watchFolders;
    for(int row = 0; row < watchModel->rowCount();row++){
        watchFolders.append(watchModel->item(row,(watchHeader.indexOf("Path")))->text());
    }

    /* If selected folder IS ALREADY in the watchModel */
    /* Import the upload log in the given folder / create new file if not exists */
    if(watchFolders.contains(folderPath))
        searchFolderLog(folderPath);
    /* Else, add a new entry to watch folders */
    else{
        QFileInfo fileInfo(folderPath);
        if (folderPath.length() > 0) {
            QList<QStandardItem *> watchRow({
                new QStandardItem(dir.dirName()),
                new QStandardItem("Queue"),
                new QStandardItem(QString::number(num_files)),
                new QStandardItem(QDateTime::currentDateTime().toString(timeFormat)),
                new QStandardItem(fileInfo.lastModified().toString(timeFormat)),
                new QStandardItem(folderPath),
            });

        this->watchModel->appendRow(watchRow);
        ui->statusBar->showMessage("Folder added to watchlist");
        ui->watchTableView->resizeColumnsToContents();
    }

    /************************ Store the scanned folders in registry *****************/
    /* Make a Json Array of the folder paths in the watchModel  */
    QJsonArray watchArr;
    QJsonObject obj;
    /* Each entry store the path to the scanned folder */
    for(int row = 0; row < watchModel->rowCount();row++){
        obj["folder_path"] = watchModel->item(row,(watchHeader.indexOf("Path")))->text();
        watchArr.append(obj);
    }
    /* Convert the Json Array to string and save to registry */
    qDebug() << "Saving watch folders to registry";
    QJsonDocument watchJsonDoc(watchArr);
    QString watchJsonString (watchJsonDoc.toJson());
    settings->setValue("lastScannedAlbums",watchJsonString);
    settings->sync();
    /**************************************************************************************/

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

void MainWindow::folderScan(){
    /* make a list of the files in the queue */
    QStringList queue;
    for(int row = 0; row < queueModel->rowCount();row++){
        queue.append(queueModel->item(row,(queueHeader.indexOf("Path")))->text());
    }

    /* iterate the files in the folder */
    for(int row = 0; row < watchModel->rowCount();row++){
         if(watchModel->item(row,(watchHeader.indexOf("Status")))->text() == "Queue" ||
                 watchModel->item(row,(watchHeader.indexOf("Status")))->text() == "Scanned"){
            QDir dir(watchModel->item(row,(watchHeader.indexOf("Path")))->text());
            qDebug() << "Scanning foldler" << dir.path();
            /* for each file in folder, if not in queue  and uploadedList, add to queue */
            QFileInfoList images = dir.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
            if (images.length() > 0 ) {
                isReady = false;
                foreach(QFileInfo i, images){
                    if( !queue.contains(i.filePath())  && !uploadedList.contains(i.filePath())){

                        QList<QStandardItem *> queueRow({
                            new QStandardItem(i.fileName()),
                            new QStandardItem("None"),
                            new QStandardItem("Queue"),
                            new QStandardItem(i.birthTime().toString(timeFormat)),
                            new QStandardItem(i.lastModified().toString(timeFormat)),
                            new QStandardItem(i.filePath())
                        });

                        this->queueModel->appendRow(queueRow);
                        ui->statusBar->showMessage("Queue added");
                        ui->queueTableView->resizeColumnsToContents();
                    }
                }
                isReady = true;
            }
            watchModel->item(row,(watchHeader.indexOf("No. Files")))->setText(QString::number(images.length()));
            watchModel->item(row,(watchHeader.indexOf("Status")))->setText("Scanned");
            connect(gphoto,SIGNAL(mediaCreated(QString)),this,SLOT(updateUploadedList(QString)));
            }
     }
}

/************************** END **************************************/


/************************** Album Methods **************************************/
void MainWindow::showCreateAlbumDialog() {
    if(gphoto != nullptr){
        CreateAlbumDialog *dialog = new CreateAlbumDialog(this);
        /* Grab Id from registry */
        dialog->setExistingAlbum(loadUsedAlbum("id"),loadUsedAlbum("name"));
        dialog->show();
        connect(dialog,SIGNAL(createAlbumSignal(QString const, QString const)), this, SLOT(createAlbum(QString const, QString const)));
        connect(dialog,SIGNAL(existingAlbumSignal(QString const &)),this,SLOT(linkExistingAlbum(QString const &)));

        /* display album name on the Main  Window once it is created */
        connect(gphoto,SIGNAL(albumIdConnected(QString const, QString const)),this,SLOT(displayAlbumName(QString const, QString const)));
        connect(gphoto,SIGNAL(albumIdChanged(QString const, QString const)),this,SLOT(displayAlbumName(QString const, QString const)));

    }else
        ui->statusBar->showMessage("Please log in before trying to create album");
}

void MainWindow::createAlbum(QString const &name, QString const &desc) {
    if (gphoto->isAuthenticated()){
        /* Create a new album */
        gphoto->SetAlbumName(name);
        gphoto->SetAlbumDescription(desc);
        gphoto->CreateAlbum();
        connect(gphoto,SIGNAL(albumCreated()),gphoto,SLOT(ShareAlbum()));
        connect(gphoto,SIGNAL(albumIdChanged(QString const &,QString const &)),this,SLOT(saveUsedAlbum(QString const &, QString const &)));

        /* Delete LastScannedFolders data from settings */
        settings->setValue("lastScannedAlbums","");

        /* Show message */
        connect(gphoto,SIGNAL(showMessage(QString const)), ui->statusBar, SLOT(showMessage(QString const)));
    }
    else{
        QString msg = "Google Photo is not authenticated. Please log in";
        ui->statusBar->showMessage(msg);
    }
}

void MainWindow::linkExistingAlbum(QString const &id){
    if (gphoto->isAuthenticated()){
        gphoto->SetTargetAlbumToUpload(id);

        /* import last sannced folder from registry */
        connect(gphoto,SIGNAL(albumIdConnected(QString const,QString const)),this,SLOT(importLastScannedFolders()));

        /* Show message */
        connect(gphoto,SIGNAL(showMessage(QString const)), ui->statusBar, SLOT(showMessage(QString const)));
    }
    else{
        QString msg = "Google Photo is not authenticated. Please log in";
        ui->statusBar->showMessage(msg);
    }
}

void MainWindow::saveUsedAlbum(QString const &id, QString const &name){
    /* save current album id to registry */
    qDebug() << "Saving album ID to registry after closing Create Album Dialog";
    QJsonObject obj;
    obj["name"] = name ;
    obj["id"] = id;
    QJsonDocument doc(obj);
    settings->setValue("lastUsedAlbum",QString(doc.toJson()));
    settings->sync();
}

QString MainWindow::loadUsedAlbum(QString const &key){
    QJsonDocument doc = QJsonDocument().fromJson(settings->value("lastUsedAlbum").toByteArray());
    QJsonObject obj = doc.object();
    return obj[key].toString();
}
/************************** END **************************************/


/************************** Lists tracking **************************************/
void MainWindow::updateUploadedList(QString const &filename){
    QFileInfo info(filename);
    QJsonObject obj;

    /* ############ make a list of the folder paths in the watchModel ############# */
    QStringList watchFolders;
    for(int row = 0; row < watchModel->rowCount();row++){
        watchFolders.append(watchModel->item(row,(watchHeader.indexOf("Path")))->text());
    }

    /* If this photo was in a watch folder, set folder_watch == true */
    QString folder_path = info.dir().path();
    obj["folder_watch"] = (watchFolders.contains(folder_path));
    /* ####  NOTE: doing this allow importLog to populate the queue and the watchFolder correctly next run ##### */

    /* Add the remaining info of each photo to the json object*/
    obj["name"] = info.fileName();
    obj["path"] = info.filePath();
    obj["album_name"] = gphoto->GetAlbumName();
    obj["album_url"] = gphoto->GetAlbumURL();
    obj["album_id"] = gphoto->GetAlbumID();
    obj["status"] = "Completed";
    obj["date_added"] = QDateTime::currentDateTime().toString(timeFormat);
    obj["url"] = gphoto->GetUploadedPhotoURL();

    /* Prevent duplicate entries in the list */
    QJsonValue v(obj);
    /* Add to list if there is no duplicate */
    if(!uploadedListJson.contains(v)){
        /* Append the object to the Json Object List */
        uploadedListJson.append(obj);

        /* Append the file path to the uploaded list */
        uploadedList.append(filename);


        /* If a file recovered from a previous failed upload, remove the file from the failed list */
        if(uploadFailedList.contains(filename))
            uploadFailedList.remove(filename);

        /* show status */
        ui->statusBar->showMessage(filename + " is uploaded");

//        qDebug() << "After Uploaded list:" << uploadedList;
//        qDebug() << "After Uploaded list:" << uploadedListJson;
    }

    /* Update upload log */
    saveProgress();

    /* ready for the next upload */
    isReady = true;
}

void MainWindow::updateFailedList(QString const &filename){
    if(uploadFailedList.contains(filename)){
        int retries = uploadFailedList.value(filename);
        retries += 1;
        uploadFailedList.insert(filename,retries);
    }else
        uploadFailedList.insert(filename,1);
    ui->statusBar->showMessage(filename + " is NOT uploaded");
}
/************************** END **************************************/


/************************** Importing Logs **************************************/
void MainWindow::searchFolderLog(QString const &folder_path){
    // Search the current directory for upload_log.txt
    QDir folder(folder_path);
    QFile file(folder.absoluteFilePath("upload_log.txt"));

    // If found, import into uploadedList and uploadedListJson
    if(file.exists()){
            file.open(QFile::ReadOnly);
            QJsonDocument document = QJsonDocument().fromJson(file.readAll());
            file.close();
            uploadedListJson = document.array();
            uploadedList = QStringList();
            // Need to copy all the paths into uploadedList variable
            for(int i = 0; i < uploadedListJson.count();i ++){
                QJsonObject obj = uploadedListJson[i].toObject();
                uploadedList.append(obj["path"].toString());
            }

          /* Keep track of which folder is added to watchTableView from the log, keep in mind
           many entries will have the same folder, so we have to avoid duplicate entries */
          QStringList addedFolderList;

          // Now all those entries should be populated to the queue with status = Completed
            for(int i = 0; i < uploadedListJson.count();i ++){
                QJsonObject obj = uploadedListJson[i].toObject();
                QFileInfo file(obj["path"].toString());

                /* if the file exists (i.e it has not been moved since uploaded
                 * then add to queue with status == completed */
                QString temp_status;
                if (file.exists())
                    temp_status = obj["status"].toString();
                /* else, set status == File missing as an indication*/
                else
                    temp_status = "File is missing";

                /* Create the entry */
                QList<QStandardItem *> queueRow({
                    new QStandardItem(obj["name"].toString()),
                    new QStandardItem(obj["album_name"].toString()),
                    new QStandardItem(temp_status),
                    new QStandardItem(obj["date_added"].toString()),
                    new QStandardItem(file.lastModified().toString(timeFormat)),
                    new QStandardItem(obj["path"].toString())
                });

                this->queueModel->appendRow(queueRow);
                ui->queueTableView->resizeColumnsToContents();

                /*********************************************/
                /* If "folder_watch" of this photo == true,
                 * populate the watchModel view, else do nothing
                 * NOTE: doing this to distinguish queue item coming from folder
                        and queue item added manually*/
                if(obj["folder_watch"] == true){
                    QDir folder(file.dir().path());
                    QFileInfo folderInfo(folder.path());
                    int num_files = folder.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files).length();

                    /* If folder path is not in watchTable, add it*/
                    if(!addedFolderList.contains(folder.path())){
                        addedFolderList.append(folder.path());
                        /* Create the entry */
                        QList<QStandardItem *> watchRow({
                            new QStandardItem(folder.dirName()),
                            new QStandardItem("Scanned"),
                            new QStandardItem(QString::number(num_files)),
                            new QStandardItem(obj["date_added"].toString()),
                            new QStandardItem(folderInfo.lastModified().toString(timeFormat)),
                            new QStandardItem(folder.path()),
                        });

                        this->watchModel->appendRow(watchRow);
                        ui->watchTableView->resizeColumnsToContents();
                    }// Otherwise, do nothing to prevent duplicate
                }
            }
        }
        // Else, create a new file and write "Hello World"
        else{
            QFile newFile(folder_path + QString("/upload_log.txt"));
            if (newFile.open(QIODevice::WriteOnly)) {
                    newFile.write("Hello World");
                    newFile.close();
                }
                else
                    qDebug() << "failed to create new log file" << endl;
        }
}

void MainWindow::importLastScannedFolders(){
    QJsonDocument document = QJsonDocument().fromJson(settings->value("lastScannedAlbums").toByteArray());
    if (!document.isEmpty()){
        QJsonArray watchArr = document.array();
        for(int i = 0; i < watchArr.count();i++){
            QJsonObject jsonObj = watchArr[i].toObject();
            searchFolderLog(jsonObj["folder_path"].toString());
        }
    }else
        qDebug() << "Last Scanned Folder List is empty";
}

/************************** END **************************************/


/************************** Saving logs **************************************/
void MainWindow::saveProgress(){
    /* Save the master Json List to Document folder */
    saveMasterLog();

    /* Create a list of the folder paths in the watchModel */
    QStringList watchPaths;
    for(int row = 0; row < watchModel->rowCount();row++)
        watchPaths.append(watchModel->item(row,(watchHeader.indexOf("Path")))->text());

    /* Iterate through the paths */
    foreach(QString path, watchPaths){
        /* From the uploadedListJson, group the photos located in the same directory */
        QJsonArray arr;
        /* Iterate the json array uploadedListJson, if the element's path == current folder path
         * append to "arr" array */
        for(int i = 0; i < uploadedListJson.count(); i++){
            /* prevent duplicate */
            if(!arr.contains(uploadedListJson[i])){
                QJsonObject jsonObj = uploadedListJson[i].toObject();
                QFileInfo info (jsonObj["path"].toString());
                if(path == info.dir().path())
                    arr.append(jsonObj);
            }
        }
         /* "arr" should now contain elements in a specific folder, write to upload_log  */
         if(!arr.isEmpty()){
             /* open upload_log.txt */
             QFile file (path + QString("/upload_log.txt"));
             /* Whether file exists or not, overwrite the data */
             if(file.open(QIODevice::WriteOnly)){
                 QJsonDocument json_doc(arr);
                 QString json_string = json_doc.toJson();
                 file.write(json_string.toLocal8Bit());
                 file.close();
             }else
                 qDebug() << "failed to save progress in" + path << endl;
           }
      }
}

void MainWindow::importMastertLog(){
    // Behavior: import a log and populate all queueTable and watchFolder ///
    QString documentPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory);
    QString pathToLog =   documentPath + QString("PixylPushLog/upload_log.txt");
    QFile file(pathToLog);
    if(file.exists()){
            file.open(QFile::ReadOnly);
            QJsonDocument document = QJsonDocument().fromJson(file.readAll());
            file.close();
            uploadedListJson = document.array();
            uploadedList = QStringList();
            // Need to copy all the paths into uploadedList variable
            for(int i = 0; i < uploadedListJson.count();i ++){
                QJsonObject obj = uploadedListJson[i].toObject();
                uploadedList.append(obj["path"].toString());
            }

          /* Keep track of which folder is added to watchTableView from the log, keep in mind
           many entries will have the same folder, so we have to avoid duplicate entries */
          QStringList addedFolderList;

          // Now all those entries should be populated to the queue with status = Completed
            for(int i = 0; i < uploadedListJson.count();i ++){
                QJsonObject obj = uploadedListJson[i].toObject();
                QFileInfo file(obj["path"].toString());

                /* if the file exists (i.e it has not been moved since uploaded
                 * then add to queue with status == completed */
                /* else, set status == File missing as an indication*/
                QString temp_status;
                temp_status = (file.exists()) ? obj["status"].toString() : "File is missing";

                /* Create the entry */
                QList<QStandardItem *> queueRow({
                    new QStandardItem(obj["name"].toString()),
                    new QStandardItem(obj["album_name"].toString()),
                    new QStandardItem(temp_status),
                    new QStandardItem(obj["date_added"].toString()),
                    new QStandardItem(file.lastModified().toString(timeFormat)),
                    new QStandardItem(obj["path"].toString())
                });
                this->queueModel->appendRow(queueRow);
                ui->queueTableView->resizeColumnsToContents();

                /*********************************************/
                /* If "folder_watch" of this photo == true,
                 * populate the watchModel view, else do nothing
                 * NOTE: doing this to distinguish queue item coming from folder
                        and queue item added manually*/
                if(obj["folder_watch"] == true){
                    QDir folder(file.dir().path());
                    QFileInfo folderInfo(folder.path());
                    int num_files = folder.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files).length();

                    /* If folder path is not in watchTable, add it*/
                    if(!addedFolderList.contains(folder.path())){
                        addedFolderList.append(folder.path());
                        /* Create the entry */
                        QList<QStandardItem *> watchRow({
                            new QStandardItem(folder.dirName()),
                            new QStandardItem("Scanned"),
                            new QStandardItem(QString::number(num_files)),
                            new QStandardItem(obj["date_added"].toString()),
                            new QStandardItem(folderInfo.lastModified().toString(timeFormat)),
                            new QStandardItem(folder.path()),
                        });

                        this->watchModel->appendRow(watchRow);
                        ui->watchTableView->resizeColumnsToContents();
                    }// Otherwise, do nothing to prevent duplicate
                }
            }
      }
}

void MainWindow::saveMasterLog(){
    /* if log file does not exist, create a new one. Otherwise, overwrite */
    QString documentPath = QStandardPaths::locate(QStandardPaths::DocumentsLocation, QString(), QStandardPaths::LocateDirectory);
    QString logPath =   documentPath + QString("PixylPushLog/upload_log_master.txt");
    QFile jsonFile (logPath);
    if(!uploadedListJson.isEmpty()){
        if (jsonFile.open(QIODevice::WriteOnly)) {
            QJsonDocument json_doc(uploadedListJson);
            QString json_string = json_doc.toJson();
            jsonFile.write(json_string.toLocal8Bit());
            jsonFile.close();
         }
         else
             qDebug() << "failed to open save file" << endl;
    }
}

/************************** END **************************************/


/************************** Email **************************************/
void MainWindow::showEmailTemplate() {
    if(emailDialog == nullptr)
        emailDialog = new EmailTemplateDialog(this);
    emailDialog->show();

    /* Read the email.txt file */
//    QFileDialog *fileDialog = new QFileDialog(this);
//    QString emailPath = fileDialog->getOpenFileName(this, tr("Select Email file"), "", tr("Texts (*.txt)"));

    /* testing */
    QString  emailPath = "C:/Users/khuon/Documents/GitHub/PixylPush/Email.txt";


    /* Connect file watcher */
    emailWatcher = new QFileSystemWatcher (QStringList(emailPath));
    connect(emailWatcher,SIGNAL(fileChanged(const QString &)),this,SLOT(importEmailQueue(const QString &)));

}

void MainWindow::importEmailQueue(const QString &emailPath){
    if(emailDialog == nullptr){
        showEmailTemplate();
        return;
    }

    QJsonArray outArr;
    QJsonArray arr;
    QFile emailFile(emailPath);
    if(emailFile.exists()){
         emailFile.open(QFile::ReadOnly);
         QJsonDocument document = QJsonDocument().fromJson(emailFile.readAll());
         arr = document.array();
         emailFile.close();

    /* For each item, find the photo url, send the email, and add the status */
    for(int i = 0; i < arr.count(); i++){
        QJsonObject jsonItem = arr[i].toObject();
        QString status = jsonItem["Status"].toString();
        QJsonArray pathsArray = jsonItem["PhotoPaths"].toArray();
        QStringList paths;
        foreach (QJsonValue v, pathsArray){
            paths << v["path"].toString();
        }


        QString to = jsonItem["Email"].toString();
        qDebug() << paths;

        /* Must check status of each entry, if status = Queue, then process, else ignore */
        if(status == "Queue"){
            if(paths.count() > 0){
                qDebug() << "++++++++++";
//                qDebug() << emailDialog->getTo() << emailDialog->getSubject() << emailDialog->getBody();
                // send SMTP with receiver and path
                sendSMTP(emailDialog->getFrom(),
                         to,
                         emailDialog->getSubject(),
                         emailDialog->getBody(),
                         paths);
                jsonItem["Status"] = "Sent";
                outArr.push_back(jsonItem);
                qDebug() << outArr;
            }
         }else{
                if(status == "Sent")
                    continue;
              }
        }
    }

/* Write log to a separate txt */
    QFileInfo info (emailPath);
    QDir dir (info.dir());
    QFile outFile (dir.path() + "/Email.txt");
    if (outFile.open(QIODevice::WriteOnly)) {
      QJsonDocument json_doc(outArr);
      QString json_string = json_doc.toJson();

      outFile.write(json_string.toLocal8Bit());
      outFile.close();
    }

/* Write log to the same Email.txt
 BUG: writing to the file will trigger the importEmailQueue again,
 reinitialize the ourArr variable, thus writing an empty array to the file.
 Have to figure out a different way to avoid sending duplicate photos*/


//    if(emailFile.open(QFile::WriteOnly)){
//      qDebug() << outArr;
//      QJsonDocument json_doc(outArr);
//      QString json_string = json_doc.toJson();

//      emailFile.write(json_string.toLocal8Bit());
//      emailFile.close();
//    }

}

void MainWindow::sendSMTP(QString const &sender,
                          QString const &receiver,
                          QString const &sub,
                          QString const &body,
                          QStringList const &paths){

    // First create the SmtpClient object and set the user and the password.
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    smtp.setUser("info.enchanted.oc@gmail.com");
    smtp.setPassword("takemetonewheights$2020");

    // Create a MimeMessage
    MimeMessage message;
    EmailAddress from("info.enchanted.oc@gmail.com", sender);
    message.setSender(&from);


    EmailAddress to(receiver, emailDialog->getTo());
    message.addRecipient(&to);
    message.setSubject(sub);

    /* Replace the tag <> im the body */
    QString temp_body = body;
    if(gphoto != nullptr){
        if(gphoto->isAlbumReady()){
            qDebug() << gphoto->GetAlbumURL();
            QFileInfo info (paths[0]);
            temp_body.replace("<LINKS>", gphoto->GetAlbumURL());
            temp_body.replace("<ALBUMNAME>", "<h3>" + gphoto->GetAlbumName() + "</h3>");
            temp_body.replace("<DATE>", info.lastModified().toString(timeFormat) );
        }
    }


    // Now we need to create a MimeHtml object for HTML content
    MimeHtml html;
    QString img_tag;

    foreach(QString path, paths){
        QFileInfo pic(path);
        img_tag.append(QString("<img src='cid:%1'/>").arg(pic.fileName()));
    }
    html.setHtml(temp_body + img_tag);

    /* Add the inline photos */
    int padding = 5 - paths.count() ;
    QStringList temp_paths = paths;
    switch (padding){
        case 4:
             temp_paths << "" << "" << "" << "" ;
            break;
        case 3:
            temp_paths << "" << "" << "";
            break;
        case 2:
            temp_paths << "" << "";
            break;
        case 1:
            temp_paths << "";
            break;
        default:
            temp_paths << "" << "" << "" << "" ;

    }

//    qDebug() << temp_paths;
    QFileInfo pic1(temp_paths[0]);
    MimeInlineFile image1 (new QFile(pic1.filePath()));
    image1.setContentId(pic1.fileName());
    image1.setContentType("image/jpg");

    QFileInfo pic2(temp_paths[1]);
    MimeInlineFile image2 (new QFile(pic2.filePath()));
    image2.setContentId(pic2.fileName());
    image2.setContentType("image/jpg");

    QFileInfo pic3(temp_paths[2]);
    MimeInlineFile image3 (new QFile(pic3.filePath()));
    image3.setContentId(pic3.fileName());
    image3.setContentType("image/jpg");

    QFileInfo pic4(temp_paths[3]);
    MimeInlineFile image4 (new QFile(pic4.filePath()));
    image4.setContentId(pic4.fileName());
    image4.setContentType("image/jpg");

    QFileInfo pic5(temp_paths[4]);
    MimeInlineFile image5 (new QFile(pic5.filePath()));
    image5.setContentId(pic5.fileName());
    image5.setContentType("image/jpg");


    message.addPart(&html);
    message.addPart(&image1);
    message.addPart(&image2);
    message.addPart(&image3);
    message.addPart(&image4);
    message.addPart(&image5);


    // Now the email can be sended
    if (!smtp.connectToHost())
        qDebug() << "Failed to connect to host!" << endl;


    if (!smtp.login())
        qDebug() << "Failed to login!" << endl;


    if (!smtp.sendMail(message))
        qDebug() << "Failed to send mail!" << endl;

    smtp.quit();
}


/************************** END **************************************/


/************************** SMS **************************************/
void MainWindow::showSMSTemplate() {
    smsDialog = new SMSTemplateDialog(this);
    smsDialog->show();

    /* Read the sms.txt file */
//    QFileDialog *fileDialog = new QFileDialog(this);
//    QString smslPath = fileDialog->getOpenFileName(this, tr("Select SMS file"), "", tr("Texts (*.txt)"));

    /* testing */
    QString  smsPath = "C:/Users/khuon/Documents/GitHub/PixylPush/SMS.txt";


    /* Connect file watcher */
    smsWatcher = new QFileSystemWatcher (QStringList(smsPath));
    smsWatcher->disconnect();
    connect(smsWatcher,SIGNAL(fileChanged(const QString &)),this,SLOT(importSMSQueue(const QString &)));

}


void MainWindow::importSMSQueue(const QString &smsPath){
    qDebug() << "SMS...";
    if(smsDialog == nullptr){
        showSMSTemplate();
        return;
    }

    QJsonArray outArr;
    QJsonArray arr;
    QFile smsFile(smsPath);
    if(smsFile.exists()){
         smsFile.open(QFile::ReadOnly);
         QJsonDocument document = QJsonDocument().fromJson(smsFile.readAll());
         arr = document.array();
         smsFile.close();

    /* For each item, find the photo url, send the email, and add the status */
    for(int i = 0; i < arr.count(); i++){
        QJsonObject jsonItem = arr[i].toObject();
        QString status = jsonItem["Status"].toString();
        QJsonArray pathsArray = jsonItem["PhotoPaths"].toArray();
        QStringList paths;
        foreach (QJsonValue v, pathsArray){
            paths << v["path"].toString();
        }
        qDebug() << paths;


        carrier_map["ATT"] = "@txt.att.net";
        carrier_map["T-Mobile"] = "@tmomail.net";
        carrier_map["Verizon"] = "@vtext.com";

        QString to = jsonItem["Phone"].toString().replace("-","") + carrier_map.value(jsonItem["Carrier"].toString());

        qDebug() << to;

        /* Must check status of each entry, if status = Queue, then process, else ignore */
        if(status == "Queue"){
            if(paths.count() > 0){
                // send SMTP with receiver and path
                sendSMTPsms( to,
                             jsonItem["Phone"].toString(),
                             smsDialog->getBody(),
                             paths);
                jsonItem["Status"] = "Sent";
                outArr.push_back(jsonItem);
            }
         }else{
                if(status == "Sent")
                    continue;
              }
        }
    }

/* Save complettion to a text file */
    QFileInfo info (smsPath);
    QDir dir (info.dir());
    QFile outFile (dir.path() + "/SMSDone.txt");

    if (outFile.open(QIODevice::WriteOnly)) {
      QJsonDocument json_doc(outArr);
      QString json_string = json_doc.toJson();

      outFile.write(json_string.toLocal8Bit());
      outFile.close();
    }

/* Write log to the same sms.txt */
//    if(smsFile.open(QFile::WriteOnly)){
//      QJsonDocument json_doc(outArr);
//      QString json_string = json_doc.toJson();

//      smsFile.write(json_string.toLocal8Bit());
//      smsFile.close();
//    }

}


void MainWindow::sendSMTPsms( QString const &receiver,
                              QString const &guest_num,
                              QString const &body,
                              QStringList const &paths){

    // First create the SmtpClient object and set the user and the password.
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    smtp.setUser("info.enchanted.oc@gmail.com");
    smtp.setPassword("takemetonewheights$2020");

    // Create a MimeMessage
    MimeMessage message;
    EmailAddress from("info.enchanted.oc@gmail.com");
    message.setSender(&from);


    EmailAddress to(receiver);
    message.addRecipient(&to);

    /* Replace the tag <> im the body */
    QString temp_body = body;
    if(gphoto != nullptr){
        if(gphoto->isAlbumReady()){
            QFileInfo info (paths[0]);
            temp_body.replace("<LINKS>", gphoto->GetAlbumURL());
            temp_body.replace("<ALBUMNAME>", "<h3>" + gphoto->GetAlbumName() + "</h3>");
            temp_body.replace("<DATE>", info.lastModified().toString(timeFormat));
            temp_body.replace("<PHONE>", guest_num);

        }
    }


    // Now we need to create a MimeHtml object for HTML content
    MimeHtml html;
    QString img_tag;

    foreach(QString path, paths){
        QFileInfo pic(path);
        img_tag.append(QString("<img src='cid:%1'/>").arg(pic.fileName()));
    }
    html.setHtml(temp_body + img_tag);

    /* Add the inline photos */
    int padding = 5 - paths.count() ;
    QStringList temp_paths = paths;
    switch (padding){
        case 4:
             temp_paths << "" << "" << "" << "" ;
            break;
        case 3:
            temp_paths << "" << "" << "";
            break;
        case 2:
            temp_paths << "" << "";
            break;
        case 1:
            temp_paths << "";
            break;
        default:
            temp_paths << "" << "" << "" << "" ;

    }

//    qDebug() << temp_paths;
    QFileInfo pic1(temp_paths[0]);
    MimeInlineFile image1 (new QFile(pic1.filePath()));
    image1.setContentId(pic1.fileName());
    image1.setContentType("image/jpg");

    QFileInfo pic2(temp_paths[1]);
    MimeInlineFile image2 (new QFile(pic2.filePath()));
    image2.setContentId(pic2.fileName());
    image2.setContentType("image/jpg");

    QFileInfo pic3(temp_paths[2]);
    MimeInlineFile image3 (new QFile(pic3.filePath()));
    image3.setContentId(pic3.fileName());
    image3.setContentType("image/jpg");

    QFileInfo pic4(temp_paths[3]);
    MimeInlineFile image4 (new QFile(pic4.filePath()));
    image4.setContentId(pic4.fileName());
    image4.setContentType("image/jpg");

    QFileInfo pic5(temp_paths[4]);
    MimeInlineFile image5 (new QFile(pic5.filePath()));
    image5.setContentId(pic5.fileName());
    image5.setContentType("image/jpg");


    message.addPart(&html);
    message.addPart(&image1);
    message.addPart(&image2);
    message.addPart(&image3);
    message.addPart(&image4);
    message.addPart(&image5);


    // Now the email can be sended
    if (!smtp.connectToHost())
        qDebug() << "Failed to connect to host!" << endl;


    if (!smtp.login())
        qDebug() << "Failed to login!" << endl;


    if (!smtp.sendMail(message))
        qDebug() << "Failed to send mail!" << endl;

    smtp.quit();
}

/************************** END **************************************/


MainWindow::~MainWindow()
{
    delete ui;
}

