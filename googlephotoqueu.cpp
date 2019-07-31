#include "googlephotoqueu.h"

GooglePhotoQueu::GooglePhotoQueu(QObject *parent) : QObject(parent)
{

}

void GooglePhotoQueu::begin(){
    //Open log file
    OpenLog();

    //Create object and a shared album
    p = new GooglePhoto();
    p->SetAlbumName("Hey Yo");
    p->SetAlbumDescription("Something fun");
    connect(p,SIGNAL(authenticated()),p,SLOT(CreateAlbum()));
    connect(p,SIGNAL(albumCreated()),p,SLOT(ShareAlbum()));


    // Check directory every 15 second for new photo
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(CheckCameraFolder()));
    timer1->start(4000);

    // Check the upload list every 1 second for photo to upload
    timer2 = new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(CheckUploadList()));
    timer2->start(1000);

//    uploadedList = QStringList({"me1.jpg", "rdm1.jpg", "key1.jpg"});
//    QTimer::singleShot(90000,this,SLOT(CloseLog()));


    // Email
//    email = new GMAIL();
//    email->SetToEmail("khuong.dinh.ng@gmail.com");
//    email->SetFromEmail("khuongnguyensac@gmail.com");
//    connect(p,SIGNAL(albumShared(QString)),email,SLOT(SetAlbumURL(QString)));
//    connect(email,SIGNAL(linkReady()),email,SLOT(SendEmail()));

}

void GooglePhotoQueu::CreateNewAlbum(){
    //Create object and a shared album
    p = new GooglePhoto();
    p->SetAlbumName("Hey Yo");
    p->SetAlbumDescription("Something fun");
    connect(p,SIGNAL(authenticated()),p,SLOT(CreateAlbum()));
    connect(p,SIGNAL(albumCreated()),p,SLOT(ShareAlbum()));
}

void GooglePhotoQueu::CameraFolderTimer(){
    // Check directory every 4 second for new photo
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(CheckCameraFolder()));
}

void GooglePhotoQueu::CameraFolderTimerStart(){
    timer1->start(4000);
}

void GooglePhotoQueu::CameraFolderTimerStop(){
    timer1->stop();
}

void GooglePhotoQueu::UploadListTimer(){
    // Check the upload list every 1 second for photo to upload
    timer2 = new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(CheckUploadList()));
}

void GooglePhotoQueu::UploadListTimerStart(){
    timer2->start(1000);
}

void GooglePhotoQueu::UploadListTimerStop(){
    timer2->stop();
}

void GooglePhotoQueu::OpenLog(){
    qDebug() << "Opening upload log";
    QFile jsonFile(pathToLog);
    if(jsonFile.exists()){
        jsonFile.open(QFile::ReadOnly);
        QJsonDocument document = QJsonDocument().fromJson(jsonFile.readAll());
        jsonFile.close();
        object = document.object();
//        qDebug() << object;
        QJsonArray arr = object["uploaded_photo"].toArray();
//        qDebug() << arr;
        foreach(QJsonValue i, arr){
            uploadedList.append(i.toString());
        }
        qDebug() << uploadedList;
    }
    else{
        qDebug() << "No log file";
    }
}

void GooglePhotoQueu::CloseLog(){
    qDebug() << "Closing log";
    QFile jsonFile(pathToLog);
    if(jsonFile.exists()){
        if (jsonFile.open(QIODevice::WriteOnly)) {
            QJsonArray arr;
            foreach(QString s, uploadedList){
                arr.append(QJsonValue(s));
            }
            object["uploaded_photo"] = arr;
            qDebug() << object;

            QJsonDocument json_doc(object);
            QString json_string = json_doc.toJson();

            jsonFile.write(json_string.toLocal8Bit());
            jsonFile.close();
        }
        else{
            qDebug() << "failed to open save file" << endl;
            return;
        }
      }
}

void GooglePhotoQueu::CheckCameraFolder(){
    qDebug() << "Checking camera folder...";
    /* Get the names of all the photo in the folder */
    camera_folder = new QDir(camera_folder_path);
    images = camera_folder->entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);

    foreach(QString filename, images){
    /* If photo NOT in the upload list and the uploaded list, add to upload list*/
        if(!uploadList.contains(filename) && !uploadedList.contains(filename) && isReady){
            uploadList.append(filename);
            isReady = false;
            qDebug() << "Upload list:" << uploadList;
            qDebug() << "Before Uploaded list:" << uploadedList;
        }

    /* Otherwise, do nothing */
    }
}

void GooglePhotoQueu::CheckUploadList(){
//    qDebug() << "Checking upload list...";
    /* Upload 1 item from the upload list, and write the file name
     * to the log and do nothing else until the next cycle */

    if(!uploadList.isEmpty() && p->isAlbumReady() && !p->isUploading()){
//        qDebug() << "album ready:"<< p->albumReady << "| uploading:" << p->isUploading();
        QString file = camera_folder_path + "/" + uploadList.takeFirst();
        qDebug() << "Uploading" << file;
        p->UploadPhoto(file);
        connect(p,SIGNAL(mediaCreated(QString)),this,SLOT(UpdateUploadedList(QString)));
    }

}

void GooglePhotoQueu::UpdateUploadedList(QString filename){
    QString curr_location = camera_folder_path + "/";
    if(filename.contains(curr_location)){
        filename.remove(curr_location);
    }
    uploadedList.append(filename);
    qDebug() << "After Uploaded list:" << uploadedList;
    isReady = true;
}

//void GooglePhotoQueu::foo(QString){
//    camera_folder = new QDir(camera_folder_path);
//    images = camera_folder->entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
//    foreach(QString filename, images){
//        /* define file path */
//        QString curr_location = camera_folder_path + "/" + filename;
//        QString new_location = uploaded_folder_path + "/" + filename;

//        /* ensure no upload is in progress */

//        /* upload each photo to Google Photo */
//        p->UploadPhoto(curr_location);

        /* move it to the uploaded folder*/
        /* if file exist, copy wont work. Check and make sure there is no duplicate */
//        if (QFile::exists(new_location))
//        {
//            QFile::remove(new_location);
//        }

//        /* move file to the new location */
//        QFile::copy(curr_location, new_location);
//        QFile::remove(curr_location);
//        }
//        QTimer::singleShot(2000,p,SLOT(CreateMultipleMediaInAlbum()));
//        qDebug() <<  uploaded_folder_path + "/" + filename;
//       qDebug() <<  camera_folder->absoluteFilePath(filename);

//}

