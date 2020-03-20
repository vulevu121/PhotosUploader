#include "MainWindow.h"
#include "ui_MainWindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setRange(0,100);
    ui->progressBar->hide();

    QSettings settings("Pixyl", "PixylPush");
    settingsDialog = new SettingsDialog(this);
    settings.sync();
    connect(settingsDialog, SIGNAL(settingsSaved()), this, SLOT(syncSettings()));


    m_db = new DBmanager(this);

    ui->queueTableView->setModel(m_db->getPhotoTable());
    ui->queueTableView->resizeColumnsToContents();
    ui->queueTableView->show();

    ui->watchTableView->setModel(m_db->getWatchedTable());
    ui->watchTableView->resizeColumnsToContents();
    ui->watchTableView->show();

    ui->emailTableView->setModel(m_db->getEmailTable());
    ui->emailTableView->resizeColumnsToContents();
    ui->emailTableView->show();

    ui->smsTableView->setModel(m_db->getSMSTable());
    ui->smsTableView->resizeColumnsToContents();
    ui->smsTableView->show();

    connect(ui->actionResume, SIGNAL(triggered()), this, SLOT(showErrMsg()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(showErrMsg()));

    connect(ui->addQueueButton, SIGNAL(clicked()), this, SLOT(showErrMsg()));
    connect(ui->removeQueueButton, SIGNAL(clicked()), this, SLOT(removeQueue()));
    connect(ui->clearQueueButton, SIGNAL(clicked()), this, SLOT(clearQueue()));

    connect(ui->addFolderButton, SIGNAL(clicked()), this, SLOT(showErrMsg()));
    connect(ui->removeFolderButton, SIGNAL(clicked()), this, SLOT(removeFolder()));
    connect(ui->clearWatchlistButton, SIGNAL(clicked()), this, SLOT(clearFolders()));


    connect(ui->addEmailButton, SIGNAL(clicked()), this, SLOT(showErrMsg()));
    connect(ui->removeEmailButton, SIGNAL(clicked()), this, SLOT(removeEmailQueue()));
    connect(ui->clearEmailButton, SIGNAL(clicked()), this, SLOT(clearEmailQueue()));

    connect(ui->addSmsButton, SIGNAL(clicked()), this, SLOT(showErrMsg()));
    connect(ui->removeSmsButton, SIGNAL(clicked()), this, SLOT(removeSMSQueue()));
    connect(ui->clearSmsButton, SIGNAL(clicked()), this, SLOT(clearSMSQueue()));



    connect(ui->actionCreateAlbum, SIGNAL(triggered()), this, SLOT(showCreateAlbumDialog()));
    connect(ui->actionLogIn,SIGNAL(triggered()),this,SLOT(googleLogIn()));
    connect(ui->actionLogOut,SIGNAL(triggered()),this,SLOT(googleLogOut()));
    connect(ui->actionSetting, SIGNAL(triggered()), this->settingsDialog, SLOT(show()));

    connect(ui->actionEmail, SIGNAL(triggered()), this, SLOT(showEmailTemplate()));
    connect(ui->actionSMS, SIGNAL(triggered()), this, SLOT(showSMSTemplate()));

    connect(m_db,SIGNAL(showMessage(QString const &)),ui->statusBar,SLOT(showMessage(QString const &)));


    /* disable some options */
    disableLogOutBtn();
    disableCreateAlbumBtn();

    /* Start scan on start up option */
    if(settings.value("startScanningStartup").toBool())
        resumeQueue();


    /* Initialize the scan timers */
    queueTimerInit();
    folderTimerInit();
//    saveTimerInit();

    emailDialog = new EmailTemplateDialog(this);
    smsDialog = new SMSTemplateDialog(this);

    ui->dockWidget->hide();
}


/************** Progress Bar ********************/
void MainWindow::showProgressBar(){
    if(thread == nullptr){
        ui->progressBar->show();
        thread = new QThread();
        worker = new Worker; // Do not set a parent. The object cannot be moved if it has a parent.
        worker->moveToThread(thread);

        connect(thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
        connect(thread, SIGNAL(started()), worker, SLOT(work()));
        connect(worker, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
        thread->start();
    }
}

void MainWindow::hideProgressBar(){
    if(thread != nullptr){
        worker->stopWork();
        thread->exit();
        thread = nullptr;
        ui->progressBar->hide();
    }
}
/*********************** END ********************/

void MainWindow::showErrMsg(){
    if(gphoto == nullptr){
        msgBx.setText("Please Log In First !");
        msgBx.exec();
    }
    else{
            if(!gphoto->isAlbumReady()){
                msgBx.setText("Please Create An Album First !");
                msgBx.exec();
            }
        }
}


void MainWindow::disableCreateAlbumBtn(){
    ui->actionCreateAlbum->setEnabled(false);
    ui->actionCreateAlbum->setIcon(colorIcon(":/icon/photo_album", grey));
}

void MainWindow::enableCreateAlbumBtn(){
    ui->actionCreateAlbum->setEnabled(true);
    ui->actionCreateAlbum->setIcon(colorIcon(":/icon/photo_album", white));
}

void MainWindow::disableLogInBtn(){
    ui->actionLogIn->setEnabled(false);
    ui->actionLogIn->setIcon(colorIcon(":/icon/log_in", grey));
}

void MainWindow::enableLogInBtn(){
    ui->actionLogIn->setEnabled(true);
    ui->actionLogIn->setIcon(colorIcon(":/icon/log_in", white));
}

void MainWindow::disableLogOutBtn(){
    ui->actionLogOut->setEnabled(false);
    ui->actionLogOut->setIcon(colorIcon(":/icon/log_out", grey));
}

void MainWindow::enableLogOutBtn(){
    ui->actionLogOut->setEnabled(true);
    ui->actionLogOut->setIcon(colorIcon(":/icon/log_out", white));
}

void MainWindow::displayAlbumName(QString const &id, QString const &name){
    ui->albumNameButton->setText(name);
}

void MainWindow::enableResume(){
    ui->actionResume->disconnect();
    connect(ui->actionResume,SIGNAL(triggered()),this,SLOT(resumeQueue()));
}
void MainWindow::disableResume(){
    ui->actionResume->disconnect();
    connect(ui->actionResume,SIGNAL(triggered()),this,SLOT(showErrMsg()));
}
void MainWindow::enableStop(){
    ui->actionStop->disconnect();
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(stopQueue()));
}

void MainWindow::disableStop(){
    ui->actionStop->disconnect();
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(showErrMsg()));

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
    connect(auth,SIGNAL(authenticated(QString const)),this,SLOT(enableResume()));
    connect(auth,SIGNAL(authenticated(QString const)),this,SLOT(enableStop()));
}

void MainWindow::googleLogOut(){
    if(auth != nullptr){
        ui->statusBar->showMessage("Logged out. Queue cleared. Watch folder removed.");
        stopQueue();
        clearQueue();
        clearFolders();

        /* disable album create button & enable login button*/
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(disableLogOutBtn()));
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(enableLogInBtn()));
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(disableCreateAlbumBtn()));
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(disableResume()));
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(disableStop()));

        auth->deleteCookies();
        auth->quit();
        deleteAllObjects();

    }else
        ui->statusBar->showMessage("You are not currently logged in.");
}

void MainWindow::deleteAllObjects(){
    gphoto = nullptr;
    auth = nullptr;
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
//        connect(ui->MainWindow,&QWidget::destroyed,this,MainWindow::saveUploadLog()));
    }

}
/************************** End **************************************/

/************************** Timer **************************************/
void MainWindow::saveTimerInit(){
    qDebug() << "Save Timer initiating...";
    saveTimer = new QTimer(this);
    saveTimer->start(30000);
//    connect(saveTimer,SIGNAL(timeout()),this,SLOT(exportEmailLog()));
//    connect(saveTimer,SIGNAL(timeout()),this,SLOT(exportSMSLog()));

}



void MainWindow::queueTimerStart(){
//    qDebug() << "queue timer start";
    queueTimer->start(settings->value("scanningInterval", "10").toInt() * 1000);
}

void MainWindow::queueTimerStop(){
//    qDebug() << "queue timer stop";
    queueTimer->stop();
}

void MainWindow::queueTimerInit(){
    queueTimer = new QTimer(this);
    /* upload photo queue. Run the first time and then connect the timeout signal for the following call */
    connect(queueTimer,SIGNAL(timeout()),this,SLOT(uploadQueue()));
    /* show progress bar */
    /* send photos to guests */
    /* email */
    connect(queueTimer,SIGNAL(timeout()),this,SLOT(emailGuests()));
    /* sms */
    connect(queueTimer,SIGNAL(timeout()),this,SLOT(smsGuests()));

}

void MainWindow::folderTimerStart(){
//    qDebug() << "folder timer start";
    folderTimer->start(settings->value("scanningInterval", "10").toInt() * 1000); // convert to ms
}

void MainWindow::folderTimerStop(){
//    qDebug() << "folder timer stop";
    folderTimer->stop();
}

void MainWindow::folderTimerInit(){
    folderTimer = new QTimer(this);
    connect(folderTimer,SIGNAL(timeout()),this,SLOT(scanFolder()));
}

void MainWindow::resumeQueue(){
    if(queueTimer != nullptr && folderTimer != nullptr){
        queueTimerStart();
        folderTimerStart();
        ui->statusBar->showMessage("Scanning resumed");
        ui->actionResume->setIcon(colorIcon(":/icon/resume", QColor(0, 255, 0)));
        uploadQueue();
        showProgressBar();
     }else
        qDebug() << "Unable to resume scan. Timer is not initialized";
}

void MainWindow::stopQueue(){
    if(queueTimer != nullptr && folderTimer != nullptr){
        queueTimerStop();
        folderTimerStop();
        ui->statusBar->showMessage("Scanning stopped");
        ui->actionResume->setIcon(colorIcon(":/icon/resume", QColor(255, 255, 255)));
        hideProgressBar();
    }else
        qDebug() << "Unable to stop scan. Timer is not initialized";
}

/************************** END **************************************/


/************************** Queue **************************************/
void MainWindow::addQueue() {
   ui->queueTableView->update();
   /* add a file to queue */
    QString now = QDateTime::currentDateTime().toString(timeFormat);
    QFileDialog *fileDialog = new QFileDialog(this);
    QStringList paths = fileDialog->getOpenFileNames(this, tr("Select folder"), "", tr("Images (*.png *.jpg)"));
    QString album = gphoto->GetAlbumName();
    if (paths.count()>0) {
        foreach(QString path, paths){
            QFileInfo fileInfo(path);
            m_db->addPhoto(fileInfo.fileName(),
                           album,
                           "Queue",
                           now,
                           fileInfo.lastModified().toString(timeFormat),
                           path);
        }
        ui->queueTableView->resizeColumnsToContents();
    }
    else
        ui->statusBar->showMessage("No selected files");
}



void MainWindow::removeQueue() {
    QModelIndexList selectedRows = ui->queueTableView->selectionModel()->selectedRows();
    foreach (QModelIndex index, selectedRows)
        m_db->removePhoto(index.row());

    ui->statusBar->showMessage("Queue removed");
}

void MainWindow::clearQueue() {
    m_db->clearPhoto();
    ui->statusBar->showMessage("Queue cleared");
}

void MainWindow::uploadQueue(){
    if(gphoto != nullptr){
        /* Iterate through the rows in the model, if status is Queue, upload photo,
          * If status is Completed, pass*/
        QSqlTableModel *model = m_db->getPhotoTable();
        /* Progress bar display/hide */
        QModelIndexList queueList = model->match(model->index(0,model->fieldIndex("status")),Qt::DisplayRole,"Queue",-1);
        if(queueList.length() == 0)
            hideProgressBar();


        /* Process photo queue */
         for(int row = 0; row < model->rowCount();row++){
                 QFileInfo fileInfo (model->data(model->index(row,model->fieldIndex("path"))).toString());
                 if(model->data(model->index(row,model->fieldIndex("status"))).toString() == "Queue"){
                         if(gphoto->isAlbumReady() && !gphoto->isUploading()){
                                /* If 1200px folder exists, upload the file in there */
                                QDir fileDir = fileInfo.dir();
                                if(fileDir.exists("1200px")){
                                    /* Update view */
                                    m_db->setPhotoStatus(row,"Uploading");
                                    m_db->setAlbumName(row,gphoto->GetAlbumName());
//                                    qDebug() << "Uploading" << fileInfo.absoluteFilePath() << "in /1200px";
                                    gphoto->UploadPhoto(fileDir.absolutePath()+ "/1200px/"+ fileInfo.fileName());
                                    ui->statusBar->showMessage("Uploading " + fileInfo.fileName());
                                    connect(gphoto,SIGNAL(mediaCreated(QString const)),this,SLOT(setQueueStatusComplete(QString const)));
                                    connect(gphoto,SIGNAL(mediaCreateFailed(QString const)),this,SLOT(setQueueStatusFailed(QString const)));
                                    }
                         }
                 }
             }
        }else
            qDebug() << "Upload error. Google account not logged in";
}

void MainWindow::setQueueStatusComplete(QString const &filePath){
    m_db->setPhotoStatusByPath(filePath,"Completed");
    ui->queueTableView->update();
}

void MainWindow::setQueueStatusFailed(QString const &filePath){
    m_db->setPhotoStatusByPath(filePath,"Failed");
    ui->queueTableView->update();
    elapsedTime.singleShot(settings->value("onErrorAttemptInterval","10").toInt(),this,SLOT(resetQueueStatus()));
}


void MainWindow::resetQueueStatus(){
    /* Set all "Failed" to "Queue" status*/
    QSqlTableModel *model = m_db->getPhotoTable();
    for(int row = 0; row < model->rowCount();row++){
            if(model->record(row).value("status").toString() == "Failed"){
                m_db->setPhotoStatus(row,"Queue");
            }
    }
    elapsedTime.stop();
    ui->queueTableView->update();
}


/************************** END **************************************/


/************************** Folder **************************************/
void MainWindow::addFolders(){
    QFileDialog *fileDialog = new QFileDialog(this);
    QString folderPath = fileDialog->getExistingDirectory(this, tr("Select Event Folder"), "");

    /* Use this watcher to catch changes to the folders added to watch model
    * Triggered when SMS.txt, Email.txt change or when file is added/modified in the provided directory*/
    watcher = new QFileSystemWatcher (this);
    watcher->addPath(folderPath);

    /* if "print" and "camera" folders do not exist in this directory, add the directory
     * to watch model like normal. Else, add both Prints, Camera, import sms and email txt, and download AlbumQR*/
    /* create QDir for "camera" and "print" folders */
    QDir print (folderPath + "/Prints");
    QDir camera (folderPath + "/Camera");

    if(print.exists() && camera.exists()){
        /* add the folder and settings */
        addFolder(print.path());
        addFolder(camera.path());


        /* Import SMS.txt */
        importToSMSModel(folderPath + "/SMS.txt");

        /* Import Email.txt */
        importToEmailModel(folderPath + "/Email.txt");

        /* Emit signal folder is added (for saveQR function. MUST BE emited after downloadQR above) */
        emit folderAdded(folderPath);

    }else{
        addFolder(folderPath);
    }

}

void MainWindow::scanDirectory(QString const &path){
    importToSMSModel(path + "/SMS.txt");
    importToEmailModel(path + "/Email.txt");

}

void MainWindow::addFolder(QString const &folderPath) {
    QDir dir(folderPath);
    /* User should select the parent directory of the "1200px" only */
    if(dir.dirName() == "1200px"){
        msgBx.setText("Please Do Not Select \"1200px\" folder. Select The Parent Directory!");
        msgBx.exec();
            /* else, "1200px" dir does not exists, create it, scale all photos in the current dir and place them in there */
        }else{
                if(!dir.mkdir("1200px")){
                    qDebug() << "1200px folder is created in" << dir.path();
                }
                /* Scale all images in this folder and put them in "1200px" */
                scaleImages(dir);

                /* Get a count of the photos in 1200px. Add current directory to watchModel */
                QDir dir1200 (dir.absolutePath() + "/1200px");
                int num_files = dir1200.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files).length();
                QFileInfo fileInfo(folderPath);
                QString now = QDateTime::currentDateTime().toString(timeFormat);

                m_db->addWatched(dir.dirName(),
                                 "Queue",
                                  num_files,
                                  now,
                                  fileInfo.lastModified().toString(timeFormat),
                                  folderPath);
                ui->watchTableView->resizeColumnsToContents();
    }
}


void MainWindow::removeFolder() {
    /* Delete selected row */
    QModelIndexList selectedRows = ui->watchTableView->selectionModel()->selectedRows();
    foreach (QModelIndex index, selectedRows)
            m_db->removeWatched(index.row());
    ui->statusBar->showMessage("Folder(s) removed from watchlist");
}


void MainWindow::clearFolders() {
    m_db->clearWatched();
    ui->statusBar->showMessage("Watchlist cleared");
}

void MainWindow::scanFolder(){
    if(gphoto != nullptr){
//        qDebug() << "Scanning folder...";
        QSqlTableModel *model = m_db->getWatchedTable();

        /* iterate the files in the folder */
        for(int row = 0; row < model->rowCount();row++){
             if(model->record(row).value(model->fieldIndex("status")).toString() == "Queue" ||
                     model->record(row).value(model->fieldIndex("status")).toString() == "Scanned"){
                QDir dir(model->record(row).value(model->fieldIndex("path")).toString());
                QString now = QDateTime::currentDateTime().toString(timeFormat);
                /* Scale images to 1200px */
                scaleImages(dir);

                    QFileInfoList images = dir.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
                    // Need to figure out how to NOT add duplicate to photo queue */
//                    qDebug() << "num file" << images.length() << model->record(row).value(model->fieldIndex("num_file")).toInt();
                    if (images.length() >0) {
                        foreach(QFileInfo i, images){
                            m_db->addPhoto(i.fileName(),
                                           gphoto->GetAlbumName(),
                                           "Queue",
                                           now,
                                           i.lastModified().toString(timeFormat),
                                           i.filePath());
                         }
                        }
                        m_db->setWatchedNumFile(row,images.length());
                        m_db->setWatchedStatus(row,"Scanned");
                    }
                }

    }else
        qDebug() << "Scan error. Google account not logged in";
}

//void MainWindow::addToWatchModel(QList<QStandardItem *> watchRow){
//    /* if no duplicate, add to watchModel */
//    QString folderPath = watchRow.at(watchHeader.indexOf("Path"))->text();
//    if((watchModel->findItems(folderPath,Qt::MatchExactly,watchHeader.indexOf("Path"))).count() == 0){
//        qDebug() << "Adding to watch model";
//        this->watchModel->appendRow(watchRow);
//        ui->statusBar->showMessage("Folder added to watchlist");
//        ui->watchTableView->resizeColumnsToContents();

//        /************************ Store the scanned folders in registry *****************/
//        /* folder path to be processed */
//        QJsonObject obj{
//            {"folder_path",folderPath}
//        };

//        /* retrive the list from settings
//        * NOTE: No error even if the LastUsedAlbum in Registry is empty*/
//        QJsonDocument document = QJsonDocument().fromJson(settings->value("lastScannedFolders").toByteArray());
//        QJsonArray watchArr = document.array();
//        if(!watchArr.contains(obj)){
//            watchArr << obj;
//            /* Convert the Json Array to string and save to registry */
//            qDebug() << "Saving watch folder to registry";
//            QJsonDocument watchJsonDoc(watchArr);
//            QString watchJsonString (watchJsonDoc.toJson());
//            settings->setValue("lastScannedFolders",watchJsonString);
//            settings->sync();
//            /**************************************************************************************/
//        }else
//            qDebug() << "folder_path already exisited in lastScannedFolders";
//    }
//    else
//        qDebug() << "Found watch model duplicate";
//}


/************************** END **************************************/

/************************** Scaling Images **************************************/

/* scale image to 1200px, save to "1200px" folder in the current directory */
void MainWindow::scaleImage(QString const &filePath){
    QFileInfo info(filePath);

    QImage image(filePath);
    image.scaledToWidth(1200,Qt::TransformationMode::FastTransformation);

    QString resultedPath = info.path()+"/1200px/"+ info.fileName();
    QFileInfo resultInfo (resultedPath);
    if(!resultInfo.exists()){
        if(image.save(resultedPath)){
            qDebug() << "Scale" << filePath << "successful";
        }
    }
}

void MainWindow::scaleImages(QDir &dir){
    QStringList paths = dir.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    foreach(QString path, paths){
//        qDebug() << dir.absoluteFilePath(path);
        scaleImage(dir.absoluteFilePath(path));
    }
}
/************************** END **************************************/


/************************** Album Methods **************************************/
void MainWindow::enableAddButtons(){
    ui->addQueueButton->disconnect();
    ui->addFolderButton->disconnect();
    ui->addEmailButton->disconnect();
    connect(ui->addQueueButton, SIGNAL(clicked()), this, SLOT(addQueue()));
    connect(ui->addFolderButton, SIGNAL(clicked()), this, SLOT(addFolders()));
    /* Email */
    connect(ui->addEmailButton,SIGNAL(clicked()),this,SLOT(addEmailQueue()));
    connect(ui->addEmailRowButton,SIGNAL(clicked()),this,SLOT(addEmailRow()));
    connect(ui->emailToolButton,SIGNAL(clicked()),this,SLOT(selectFileToEmail()));

    /* SMS */
    connect(ui->addSmsButton,SIGNAL(clicked()),this,SLOT(addSMSQueue()));
    connect(ui->addSmsRowButton,SIGNAL(clicked()),this,SLOT(addSMSRow()));
    connect(ui->smsToolButton,SIGNAL(clicked()),this,SLOT(selectSMSFile()));
}

void MainWindow::disableAddButtons(){
    ui->addQueueButton->disconnect();
    ui->addFolderButton->disconnect();
    ui->addEmailButton->disconnect();
    connect(ui->addQueueButton, SIGNAL(clicked()), this, SLOT(showErrMsg()));
    connect(ui->addFolderButton, SIGNAL(clicked()), this, SLOT(showErrMsg()));
    connect(ui->addEmailButton, SIGNAL(clicked()), this, SLOT(showErrMsg()));
    connect(ui->addSmsButton,SIGNAL(clicked()),this,SLOT(showErrMsg()));

}

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

        /* dowload & save QR code when album url is ready and folderAdded signal is emitted */
        connect(gphoto,SIGNAL(existingUrlReady(QString const &)),this,SLOT(downloadQR(QString const &)));
        connect(gphoto,SIGNAL(albumShared(QString const &)),this,SLOT(downloadQR(QString const &)));


        /* enable the Add buttons after album is created */
        connect(gphoto,SIGNAL(existingUrlReady(QString const &)),this,SLOT(enableAddButtons()));
        connect(gphoto,SIGNAL(albumShared(QString const &)),this,SLOT(enableAddButtons()));

        /* disable the Add buttons after loggout */
        connect(auth,SIGNAL(unauthenticated()),this,SLOT(disableAddButtons()));

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

        /* Show message */
        connect(gphoto,SIGNAL(showMessage(QString const)), ui->statusBar, SLOT(showMessage(QString const)));
    }
    else{
        QString msg = "Google Photo is not authenticated. Please log in";
        ui->statusBar->showMessage(msg);
    }
}



QString MainWindow::loadUsedAlbum(QString const &key){
    QJsonDocument doc = QJsonDocument().fromJson(settings->value("lastUsedAlbum").toByteArray());
    QJsonObject obj = doc.object();
    return obj[key].toString();
}
/************************** END **************************************/




/************************** Importing Logs **************************************/
//void MainWindow::importUploadLog(QDir dir){
//    /* If log exists, import into uploadedList and uploadedListJson */
//    if(dir.exists("upload_log.txt")){
//        QFile file(dir.filePath("upload_log.txt"));
//        file.open(QFile::ReadOnly);
//        QJsonDocument document = QJsonDocument().fromJson(file.readAll());
//        file.close();
//        uploadedListJson = document.array();

//        /* Need to copy all the paths into uploadedList variable for global use */
//        for(int i = 0; i < uploadedListJson.count();i ++){
//            QJsonObject obj = uploadedListJson[i].toObject();
//            uploadedList.append(obj["path"].toString());
//        }
//    }
//}

//void MainWindow::logToModel(){
//    /* processing the upload_log.txt if it is not empty */
//    if(!uploadedListJson.empty()){
//        // Now all those entries should be populated to the queue with status = Completed
//        for(int i = 0; i < uploadedListJson.count();i ++){
//            QJsonObject obj = uploadedListJson[i].toObject();
//            QFileInfo file(obj["path"].toString());

//            /* if the file exists (i.e it has not been moved since uploaded
//             * then add to queue with status == completed */
//            QString temp_status;
//            if (file.exists())
//                temp_status = obj["status"].toString();
//            /* else, set status == File missing as an indication*/
//            else
//                temp_status = "File is missing";

//            /* Create the entry */
//            QList<QStandardItem *> queueRow({
//                new QStandardItem(obj["name"].toString()),
//                new QStandardItem(obj["album_name"].toString()),
//                new QStandardItem(temp_status),
//                new QStandardItem(obj["date_added"].toString()),
//                new QStandardItem(file.lastModified().toString(timeFormat)),
//                new QStandardItem(obj["path"].toString())
//            });
//            /* auto prevent duplicate */
//            addToQueueModel(queueRow);

//            /*********************************************/
//            /* If "folder_watch" of this photo == true,
//             * populate the watchModel view, else do nothing
//             * NOTE: doing this to distinguish queue item coming from folder
//                    and queue item added manually*/
//            if(obj["folder_watch"] == true){
//                QDir folder(file.dir().path());
//                QFileInfo folderInfo(folder.path());
//                int num_files = folder.entryInfoList(QStringList() << "*.jpg" << "*.JPG",QDir::Files).length();
//                /* Create the entry */
//                QList<QStandardItem *> watchRow({
//                    new QStandardItem(folder.dirName()),
//                    new QStandardItem("Scanned"),
//                    new QStandardItem(QString::number(num_files)),
//                    new QStandardItem(obj["date_added"].toString()),
//                    new QStandardItem(folderInfo.lastModified().toString(timeFormat)),
//                    new QStandardItem(folder.path()),
//                });
//                /* Auto prevent duplicate */
//                addToWatchModel(watchRow);
//               }
//        }
//     }
//}
//void MainWindow::importLastScannedFolders(){
//    QJsonDocument document = QJsonDocument().fromJson(settings->value("lastScannedFolders").toByteArray());
//    if (!document.isEmpty()){
//        QJsonArray watchArr = document.array();
//        for(int i = 0; i < watchArr.count();i++){
//            QJsonObject jsonObj = watchArr[i].toObject();
//            /* Find upload_log if it is a folder_path
//             * IMPORTANT: Need to review this logic if
//             * the lastScannedFolder variable structure in Registry
//             * changes */

//            /* If the "folder_path" value is not empty, it is the "folder_path" */
//            if(jsonObj["folder_path"].toString() != ""){
//                qDebug() << "Import scanned folder";
//                addFolder(jsonObj["folder_path"].toString());
//            }
//            /* If the "album_path" value is not empty, it is the "album_path" */
//            if(jsonObj["album_path"].toString() != ""){
//                qDebug() << "import scanned email and sms log";
//                albumFolderPath = jsonObj["album_path"].toString();
//            }
//         }
//    }else
//        qDebug() << "Last Scanned Folder List is empty";
//}




/************************** END **************************************/


/************************** Email **************************************/
void MainWindow::showEmailTemplate() {
    if(emailDialog == nullptr){
        emailDialog = new EmailTemplateDialog(this);
    }
    emailDialog->show();
}

void MainWindow::selectFileToEmail(){
    QFileDialog *fileDialog = new QFileDialog(this);
    QStringList paths = fileDialog->getOpenFileNames(this, tr("Select Photos"), "", tr("Images (*.png *.jpg)"));
    this->ui->emailFileLineEdit->setText(paths.join(","));
}

void MainWindow::addEmailQueue(){
    ui->dockWidget->show();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::addEmailRow(){
    QStringList paths = ui->emailFileLineEdit->text().split(",");
    QString now = QDateTime::currentDateTime().toString(timeFormat);
    if(paths.count() > 0){
        m_db->addEmail(this->ui->emailLineEdit->text(),
                        "Queue",
                        paths.count(),
                        now,
                        paths.join(","));
        ui->emailTableView->resizeColumnsToContents();

    }else
        ui->statusBar->showMessage("No selected files");

    ui->dockWidget->hide();
}


void MainWindow::removeEmailQueue(){
    QModelIndexList selectedRows = ui->emailTableView->selectionModel()->selectedRows();
    foreach (QModelIndex index, selectedRows)
        m_db->removeEmail(index.row());

    ui->statusBar->showMessage("Email queue removed");
}

void MainWindow::clearEmailQueue(){
    m_db->clearEmail();
    ui->statusBar->showMessage("Email queue cleared");
}

/* import email.txt */
void MainWindow::importToEmailModel(const QString &emailPath){
    if(emailDialog == nullptr){
        emailDialog = new EmailTemplateDialog(this);
    }
    qDebug() << "Importing Email.txt" << emailPath;
    QJsonArray arr = openFile(emailPath);
    if(!arr.empty()){
        /* For each item, add to the queue if there is no duplicate */
        /* Interate through the email.txt, extract each json item */
        for(int i = 0; i < arr.count(); i++){
            QJsonObject jsonItem = arr.at(i).toObject();
            QJsonArray pathsArray = jsonItem["PhotoPaths"].toArray();
            QStringList paths = toStringList(pathsArray);
            QString now = QDateTime::currentDateTime().toString(timeFormat);

            m_db->addEmail(jsonItem["Email"].toString(),
                            "Queue",
                            paths.count(),
                            now,
                            paths.join(","));
            ui->emailTableView->resizeColumnsToContents();
         }
    }else
        qDebug() << "Email.txt is empty";
}



void MainWindow::emailGuests(){
    if(gphoto != nullptr){
        /* Send out email */
        QSqlTableModel *model = m_db->getEmailTable();

        for(int row = 0; row < model->rowCount();row++){
            QString status = model->data(model->index(row,model->fieldIndex("status"))).toString();
            QString to = model->data(model->index(row,model->fieldIndex("email"))).toString();
            QStringList paths = model->data(model->index(row,model->fieldIndex("path"))).toString().split(",");

            /* Send an email with 4 photo attached with this function often return "failed", but the email is sent correctly
                For now, Status will be "Completed" after 1 attempt */
            if(status == "Queue" && isDone){
                isDone = false;
                sendSMTP(emailDialog->getFrom(),
                             to,
                             emailDialog->getSubject(),
                             emailDialog->getBody(),
                             paths);
                    /* if sending is successful, change status to Completed */
                    m_db->setStatusEmail(row,"Completed");
            }
         }
    }
}




bool MainWindow::sendSMTP(QString const &sender,
                          QString const &receiver,
                          QString const &sub,
                          QString const &body,
                          QStringList paths){
    bool result = false;

    // First create the SmtpClient object and set the user and the password.
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    smtp.setUser(SMTP_user);
    smtp.setPassword(SMTP_pass);

    // Create a MimeMessage
    MimeMessage message;
//    EmailAddress from("info.enchanted.oc@gmail.com", sender);
    EmailAddress from("noreply",sender);

    message.setSender(&from);


    EmailAddress to(receiver, emailDialog->getTo());
    message.addRecipient(&to);
    message.setSubject(sub);

    /* Replace the tag <> im the body */
    QString temp_body = body;
    if(gphoto != nullptr){
        if(gphoto->isAlbumReady()){
            QFileInfo info (paths[0]);
            temp_body.replace("<ALBUMNAME>", "<h3>" + gphoto->GetAlbumName() + "</h3>");
            temp_body.replace("<LINKS>", "<a href=\"" + gphoto->GetAlbumURL() + "\">Click Here</a>");
            temp_body.replace("<DATE>", info.lastModified().toString(timeFormat) );
        }
    }
    // Now we need to create a MimeHtml object for HTML content
    MimeHtml html;
    QString img_tag;

    for(int i = 0; i < paths.length();i++){
        QFileInfo pic(paths[i]);
        /* "path" points to the scaled down version */
        paths[i] = pic.path()+"/1200px/"+pic.fileName();
        img_tag.append(QString("<img src='cid:%1'/>").arg(pic.fileName()));
    }
    html.setHtml(temp_body + img_tag);

    /* Add the inline photos */
    int padding = 4 - paths.count() ;
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




    qDebug() << temp_paths;
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


    message.addPart(&html);
    message.addPart(&image1);
    message.addPart(&image2);
    message.addPart(&image3);
    message.addPart(&image4);


    // Now the email can be send
    if (!smtp.connectToHost()){
        qDebug() << "Failed to connect to host!" << endl;
    }else if (!smtp.login()){
        qDebug() << "Failed to login!" << endl;
    }else if (!smtp.sendMail(message)){
        qDebug() << "Failed to send mail!" << endl;
    }else{
        result = true;
    }
    isDone = true;
    smtp.quit();
    return result;
}


/************************** Helper functions **************************************/


QStringList MainWindow::toStringList(QJsonArray &array){
    QStringList result;
    foreach (QJsonValue v, array){
        result << v.toString();
    }
    return result;
}

QJsonArray MainWindow::openFile(const QString &path){
    QFile file(path);
    QJsonArray result;
    if(file.exists()){
        file.open(QFile::ReadOnly);
        QJsonDocument document = QJsonDocument().fromJson(file.readAll());
        file.close();
        result = document.array();
        return result;
    }else
        return QJsonArray();
}

/************************** END **************************************/


/************************** SMS **************************************/
void MainWindow::showSMSTemplate() {
    if(smsDialog == nullptr){
        smsDialog = new SMSTemplateDialog(this);
    }
    smsDialog->show();
}

void MainWindow::selectSMSFile(){
    QFileDialog *fileDialog = new QFileDialog(this);
    QStringList paths = fileDialog->getOpenFileNames(this, tr("Select Photos"), "", tr("Images (*.png *.jpg)"));
    this->ui->smsFileLineEdit->setText(paths.join(","));
}
void MainWindow::addSMSQueue(){
    ui->dockWidget->show();
    ui->stackedWidget->setCurrentIndex(1);

}

void MainWindow::addSMSRow(){
    QString now = QDateTime::currentDateTime().toString(timeFormat);
    QStringList paths = this->ui->smsFileLineEdit->text().split(",");
    if(paths.count() > 0){
        m_db->addSMS(this->ui->phoneLineEdit->text(),
                     this->ui->carrierComboBox->currentText(),
                    "Queue",
                    paths.count(),
                    now,
                    paths.join(","));
        ui->smsTableView->resizeColumnsToContents();
    }
    else
        ui->statusBar->showMessage("No selected files");

    ui->dockWidget->hide();
}
void MainWindow::removeSMSQueue(){
    QModelIndexList selectedRows = ui->smsTableView->selectionModel()->selectedRows();
    foreach (QModelIndex index, selectedRows)
        m_db->removeSMS(index.row());

    ui->statusBar->showMessage("SMS queue removed");
}

void MainWindow::clearSMSQueue(){
    m_db->clearSMS();
    ui->statusBar->showMessage("SMS queue cleared");
}

//void MainWindow::exportSMSLog(){
//    qDebug() << "Saving sms log...";
//    QJsonArray arr;
//    for(int row = 0; row < smsModel->rowCount();row++){
//        QJsonObject obj;
//        QStringList paths = smsModel->item(row,smsHeader.indexOf("Paths"))->text().split(",");
//        QJsonArray pathsArr;
//        foreach(QString path, paths){
//            QJsonObject temp_obj {
//              {"path",path}
//            };
//            pathsArr << temp_obj;
//        }
//        obj["Phone"] = smsModel->item(row,smsHeader.indexOf("Phone"))->text();
//        obj["Carrier"] = smsModel->item(row,smsHeader.indexOf("Carrier"))->text();
//        obj["Status"] = smsModel->item(row,smsHeader.indexOf("Status"))->text() ;
//        obj["No. Files"] = smsModel->item(row,smsHeader.indexOf("No. Files"))->text();
//        obj["Last Scanned"] = smsModel->item(row,smsHeader.indexOf("Last Scanned"))->text() ;
//        obj["PhotoPaths"]= pathsArr;

//        arr << obj;
//    }

//    if(!arr.isEmpty()){
//        /* open sms log.txt */
//        QFile file (albumFolderPath + QString("/sms_log.txt"));
//        /* Whether file exists or not, overwrite the data */
//        if(file.open(QIODevice::WriteOnly)){
//            QJsonDocument json_doc(arr);
//            QString json_string = json_doc.toJson();
//            file.write(json_string.toLocal8Bit());
//            file.close();
//        }else
//            qDebug() << "failed to save progress in" + albumFolderPath << endl;
//      }
//}

/* Import SMS.txt */
void MainWindow::importToSMSModel(const QString &smsPath){
    if(smsDialog == nullptr){
        smsDialog = new SMSTemplateDialog(this);
    }

    QJsonArray arr = openFile(smsPath);
    if(!arr.empty()){
        /* For each item, add to the queue if there is no duplicate */
        /* Interate through the sms.txt, extract each json item */
        for(int i = 0; i < arr.count(); i++){
            QJsonObject jsonItem = arr.at(i).toObject();
            QJsonArray pathsArray = jsonItem["PhotoPaths"].toArray();
            QStringList paths = toStringList(pathsArray);
            QString now = QDateTime::currentDateTime().toString(timeFormat);

            m_db->addSMS(jsonItem["Phone"].toString(),
                        jsonItem["Carrier"].toString(),
                        "Queue",
                        paths.count(),
                        now,
                        paths.join(","));
            ui->smsTableView->resizeColumnsToContents();
        }
    }else
        qDebug() << "SMS.txt is epmty OR the json format is incorrect";
}


void MainWindow::smsGuests(){
    /* SHOULD HAVE wait for Ready Flag before each SMS. Implement a ready flag */

    carrier_map["ATT"] = "@txt.att.net";
    carrier_map["T-Mobile"] = "@tmomail.net";
    carrier_map["Verizon"] = "@vtext.com";
    carrier_map["Sprint"] = "@messaging.sprintpcs.com";
    carrier_map["Metro PCS"] = "@mymetropcs.com";
    carrier_map["Boost Mobile"] = "@sms.myboostmobile.com";
    carrier_map["Cricket"] = "@sms.mycricket.com";

    QSqlTableModel *m_modelSMS = m_db->getSMSTable();
    for(int row = 0; row < m_modelSMS->rowCount();row++){
        QString status = m_modelSMS->data(m_modelSMS->index(row,m_modelSMS->fieldIndex("status"))).toString();
        QString phone = m_modelSMS->data(m_modelSMS->index(row,m_modelSMS->fieldIndex("phone"))).toString();
        QString to = phone.replace("-","")
                    + carrier_map.value(m_modelSMS->data(m_modelSMS->index(row,m_modelSMS->fieldIndex("carrier"))).toString());
        QStringList paths = m_modelSMS->data(m_modelSMS->index(row,m_modelSMS->fieldIndex("path"))).toString().split(",");


        /* Status -> Completed after 1 attemp. Look at note in sendSMTP for EMAIL */
        if(status == "Queue" && isDone){
            isDone = false;
            sendSMTPsms( to,
                        phone,
                        smsDialog->getBody(),
                        paths);
                /* if sending is successful, change status to Completed */
              m_db->setStatusSMS(row,"Completed");
        }
    }
}


bool MainWindow::sendSMTPsms( QString const &receiver,
                              QString const &guest_num,
                              QString const &body,
                              QStringList paths){
    bool result = false;
    // First create the SmtpClient object and set the user and the password.
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    smtp.setUser(SMTP_user);
    smtp.setPassword(SMTP_pass);

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
            temp_body.replace("<LINKS>", "<a href=\"" + gphoto->GetAlbumURL() + "\">Click Here</a>");
            temp_body.replace("<ALBUMNAME>", "<h3>" + gphoto->GetAlbumName() + "</h3>");
            temp_body.replace("<DATE>", info.lastModified().toString(timeFormat));
            temp_body.replace("<PHONE>", guest_num);
        }
    }


    // Now we need to create a MimeHtml object for HTML content
    MimeHtml html;
    QString img_tag;

    for(int i = 0; i < paths.length();i++){
        QFileInfo pic(paths[i]);
        /* "path" points to the scaled down version */
        paths[i] = pic.path()+"/1200px/"+pic.fileName();
        img_tag.append(QString("<img src='cid:%1'/>").arg(pic.fileName()));
    }
    html.setHtml(temp_body + img_tag);

    /* Add the inline photos */
    int padding = 4 - paths.count() ;
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
        case 0:
            break;
        default:
            temp_paths << "" << "" << "" << "" ;

    }
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



    message.addPart(&html);
    message.addPart(&image1);
    message.addPart(&image2);
    message.addPart(&image3);


    // Now the sms can be send
    if (!smtp.connectToHost()){
        qDebug() << "Failed to connect to host!" << endl;
    }else if (!smtp.login()){
        qDebug() << "Failed to login!" << endl;
    }else if (!smtp.sendMail(message)){
        qDebug() << "Failed to send mail!" << endl;
    }else{
        result = true;
    }
    isDone = true;
    smtp.quit();
    return result;
}
/************************** END **************************************/

/************************** Album QR **************************************/
void MainWindow::downloadQR(QString const &url)
{
    QUrl imageUrl("https://api.qrserver.com/v1/create-qr-code/?margin=5&size=150x150&data=" + url);
    m_pImgCtrl = new FileDownloader(imageUrl, this);
    connect(m_pImgCtrl,SIGNAL(downloaded()),this,SLOT(prepQrLocation()));
}

void MainWindow::prepQrLocation(){
    /* emit a signal to trigger saveQR */
    connect(this,SIGNAL(folderAdded(QString const &)),this,SLOT(saveQR(QString const &)));
}

void MainWindow::saveQR(QString const &location){
    if(m_pImgCtrl != nullptr){
        qDebug() << "save AlbumQR.png to" << location;
        QFile file(location +"/AlbumQR.png");

        if(file.open(QIODevice::WriteOnly)){
            file.write(m_pImgCtrl->downloadedData());
            file.close();
        }else{
            qDebug() << "Unable to save album QR";
        }
    }else{
        qDebug() << "FileDownloader is not initiated";
    }
}
/************************** END **************************************/


MainWindow::~MainWindow()
{
    delete ui;
}

/****************************** Worker Class ************************/
Worker::Worker() { // Constructor
    // you could copy data from constructor arguments to internal variables here.
}

Worker::~Worker() { // Destructor
    // free resources
    stopWork();
    stop = false;
}

void Worker::work(){
        int num = 0;
        while(true){
            num = (num + 1) % 100;
            emit progress(num);
            QThread::msleep(50);
            if(stop)
                break;
        }
    }

void Worker::stopWork(){
    stop = true;
}
/************************** END **************************************/

/*************************************** EmailWorker *****************************************/
EmailWorker::EmailWorker(DBmanager *db, GooglePhoto *gphoto)
{
    QJsonDocument doc = QJsonDocument().fromJson(settings->value("savedEmailSettings").toByteArray());
    QJsonObject obj = doc.object();
    TO = obj["To"].toString();
    subject = obj["Subject"].toString();
    from = obj["From"].toString();
    body = obj["Body"].toString();
    m_db = db;
    m_gphoto = gphoto;
}

EmailWorker::~EmailWorker()
{

}

void EmailWorker::emailGuests(){
    /* SHOULD HAVE wait for Ready Flag before each Email. Implement a ready flag */
        if(m_db!= nullptr){
        /* Send out email */
        QSqlTableModel *model = m_db->getEmailTable();

        for(int row = 0; row < model->rowCount();row++){
            QString status = model->data(model->index(row,model->fieldIndex("status"))).toString();
            QString to = model->data(model->index(row,model->fieldIndex("email"))).toString();
            QStringList paths = model->data(model->index(row,model->fieldIndex("path"))).toString().split(",");

            /* Send an email with 4 photo attached with this function often return "failed", but the email is sent correctly
                For now, Status will be "Completed" after 1 attempt */
            if(status == "Queue"){
                sendSMTP(from,
                         to,
                         subject,
                         body,
                         paths);
                    /* if sending is successful, change status to Completed */
                    m_db->setStatusEmail(row,"Completed");
            }
           }
    }
}


void EmailWorker::sendSMTP(QString const &sender,
                          QString const &receiver,
                          QString const &sub,
                          QString const &body,
                          QStringList paths){

    // First create the SmtpClient object and set the user and the password.
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    smtp.setUser("info.enchanted.oc@gmail.com");
    smtp.setPassword("takemetonewheights$2020");

    // Create a MimeMessage
    MimeMessage message;
//    EmailAddress from("info.enchanted.oc@gmail.com", sender);
    EmailAddress from("noreply",sender);

    message.setSender(&from);


    EmailAddress to(receiver, TO);
    message.addRecipient(&to);
    message.setSubject(sub);

    /* Replace the tag <> im the body */
    QString temp_body = body;
    if(m_gphoto != nullptr){
        if(m_gphoto->isAlbumReady()){
            QFileInfo info (paths[0]);
            temp_body.replace("<ALBUMNAME>", "<h3>" + m_gphoto->GetAlbumName() + "</h3>");
            temp_body.replace("<LINKS>", "<a href=\"" + m_gphoto->GetAlbumURL() + "\">Click Here</a>");
            temp_body.replace("<DATE>", info.lastModified().toString(timeFormat) );
        }
    }
    // Now we need to create a MimeHtml object for HTML content
    MimeHtml html;
    QString img_tag;

    for(int i = 0; i < paths.length();i++){
        QFileInfo pic(paths[i]);
        /* "path" points to the scaled down version */
        paths[i] = pic.path()+"/1200px/"+pic.fileName();
        img_tag.append(QString("<img src='cid:%1'/>").arg(pic.fileName()));
    }
    html.setHtml(temp_body + img_tag);

    /* Add the inline photos */
    int padding = 4 - paths.count() ;
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
        case 0:
            break;
        default:
            temp_paths << "" << "" << "" << "" ;

    }




    qDebug() << temp_paths;
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


    message.addPart(&html);
    message.addPart(&image1);
    message.addPart(&image2);
    message.addPart(&image3);
    message.addPart(&image4);


    // Now the email can be send
    if (!smtp.connectToHost()){
        qDebug() << "Failed to connect to host!" << endl;
        smtp.quit();
    }else if (!smtp.login()){
        qDebug() << "Failed to login!" << endl;
        smtp.quit();
    }else if (!smtp.sendMail(message)){
        qDebug() << "Failed to send mail!" << endl;
        smtp.quit();
    }else{
        smtp.quit();
    }

}
