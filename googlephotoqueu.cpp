#include "googlephotoqueu.h"

GooglePhotoQueu::GooglePhotoQueu(QObject *parent) : QObject(parent)
{

//    begin();
}

void GooglePhotoQueu::begin(){
    //Open log file
    OpenLog();

    //Create object and a shared album
//    CreateNewAlbum();


    CameraFolderTimer();
    CameraFolderTimerStart();

    UploadListTimer();
    UploadListTimerStart();

    /* Test saving log */
//    QTimer::singleShot(30000,this,SLOT(CloseLog()));


    // Email
//    email = new GMAIL();
//    email->SetToEmail("khuong.dinh.ng@gmail.com");
//    email->SetFromEmail("khuongnguyensac@gmail.com");
//    connect(p,SIGNAL(albumShared(QString)),email,SLOT(SetAlbumURL(QString)));
//    connect(email,SIGNAL(linkReady()),email,SLOT(SendEmail()));

}
//void GooglePhotoQueu::CreateNewAlbum(){
//    //Create object and a shared album
//    p = new GooglePhoto();
//    p->SetAlbumName("Hey Yo");
//    p->SetAlbumDescription("Something fun");
//    connect(p,SIGNAL(authenticated()),p,SLOT(CreateAlbum()));
//    connect(p,SIGNAL(albumCreated()),p,SLOT(ShareAlbum()));
//}


void GooglePhotoQueu::CreateNewAlbum(QString album_name, QString album_desc){
    //Create object and a shared album
    p = new GooglePhoto();
    p->SetAlbumName(album_name);
    p->SetAlbumDescription(album_desc);
    connect(p,SIGNAL(authenticated()),p,SLOT(CreateAlbum()));
    connect(p,SIGNAL(albumCreated()),p,SLOT(ShareAlbum()));

    /* Create Timers */


}

//void GooglePhotoQueu::UploadAPhoto(QString photo_path){
//    p->UploadPhoto(photo_path);
//}

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
    timer2->start(2000);
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
        for(int i = 0; i < arr.size(); i++){
            QJsonObject jsonObj = arr[i].toObject();
            uploadedList.append(jsonObj["path"].toString());

//            qDebug() << jsonObj["name"];
        }
        qDebug() << uploadedList;
    }
    else{
        qDebug() << "No log file";
    }
}





void GooglePhotoQueu::CloseLog(){
    qDebug() << "Closing log";
    QString dir_path = camera_folder_path + "/";
    QFile jsonFile(pathToLog);
    /* if log file does not exist, create a new one. Otherwise, overwrite */
    if (jsonFile.open(QIODevice::WriteOnly)) {
            QJsonArray arr;
            QJsonObject obj;
            foreach(QString s, uploadedList){
                obj["path"] = s;
                obj["name"] = s.remove(dir_path);
                obj["album_name"] = p->GetAlbumName();
                obj["album_url"] = p->GetAlbumURL();
                obj["status"] = "Queue";
                obj["date_added"] = QDateTime::currentDateTime().toString();
                obj["url"] = p->GetUploadedPhotoURL();

                arr.append(obj);
            }
            object["uploaded_photo"] = arr;
//            qDebug() << object;

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



void GooglePhotoQueu::CheckCameraFolder(){
    qDebug() << "Checking camera folder...";
    /* Get the names of all the photo in the folder */
    camera_folder = new QDir(camera_folder_path);
    images = camera_folder->entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);


    foreach(QString filename, images){
    /* If photo NOT in the upload list and the uploaded list, add to upload list*/
        QString file_path = camera_folder->filePath(filename);
//        qDebug() << file_path;
        if(!uploadList.contains(file_path) && !uploadedList.contains(file_path) && isReady){
            uploadList.append(file_path);
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
        QString file =  uploadList.takeFirst();
        qDebug() << "Uploading" << file;
        p->UploadPhoto(file);
        connect(p,SIGNAL(mediaCreated(QString)),this,SLOT(UpdateUploadedList(QString)));
    }

}



//void GooglePhotoQueu::UpdateUploadedList(QString filename){
//    uploadedList.append(filename);
//    qDebug() << "After Uploaded list:" << uploadedList;
//    isReady = true;
//}

void GooglePhotoQueu::UpdateUploadedList(QString filename){
    QFileInfo info(filename);
    QJsonObject obj;
    obj["path"] = info.filePath();
    obj["name"] = info.fileName();
    obj["album_name"] = p->GetAlbumName();
    obj["album_url"] = p->GetAlbumURL();
    obj["status"] = "Queue";
    obj["date_added"] = QDateTime::currentDateTime().toString();
    obj["url"] = p->GetUploadedPhotoURL();

    uploadedJsonList.append(obj);
    uploadedList.append(filename);
    qDebug() << "After Uploaded list:" << uploadedList;
    isReady = true;
}





