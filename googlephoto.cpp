#include "googlephoto.h"

GooglePhoto::GooglePhoto(QObject *parent) : QObject(parent)
{
}

void GooglePhoto::NotAuthenticated(QString const &message){
    emit notAuthenticated(message);
}

void GooglePhoto::SetTargetAlbumToUpload(QString const &id){
    albumID = id;
    if( !accessToken.isEmpty()){
        GetAlbumById();
    }else{
        qDebug() << "Unable to download album info. Google photo is not authenticated";
    }

}


void GooglePhoto::GetAlbumById(){
    qDebug() << "Getting album by ID:" << albumID;
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }

    QUrl endpoint(QString("https://photoslibrary.googleapis.com/v1/albums/%1").arg(albumID));
    QNetworkRequest req(endpoint);
    req.setRawHeader("Authorization","Bearer "+ accessToken.toUtf8());
    manager->get(req);

    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(GetAlbumByIdReply(QNetworkReply*)));

}

void GooglePhoto::GetAlbumByIdReply(QNetworkReply * reply){
    if(reply->error()) {
        qDebug() << "Get album by ID Error" << reply->readAll();
        manager->disconnect();
        emit showMessage("Album NOT connected");

    } else {

        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObj = jsonDoc.object();

        albumURL = jsonObj["productUrl"].toString();
        albumName= jsonObj["title"].toString();
        manager->disconnect();
        albumReady = true;
        emit existingUrlReady(albumURL);
        emit albumIdConnected(albumID,albumName);
        emit showMessage("Album connected successfully");
     }

}


void GooglePhoto::SetAccessToken(QString const &token){
    qDebug() << "Google Photo Access Token is set";
    accessToken = token;
    emit authenticated();
}

bool GooglePhoto::isAuthenticated(){
   return !accessToken.isEmpty();
}

void GooglePhoto::SetAlbumName(QString const &name){
    albumName = name;

}

void GooglePhoto::SetAlbumDescription(QString const &note){
    albumDescription = note;
}

void GooglePhoto::SetPathToFile(QString const &path){
    qDebug() << path << "changed";
    emit pathToFileChanged(path);
}


bool GooglePhoto::isUploading(){
    return Uploading;
}

bool GooglePhoto::isAlbumReady(){
    return albumReady;

}
void GooglePhoto::UploadPhoto(QString const &pathToPic){

    qDebug() << "Uploading to existing album...";

    if(accessToken.isEmpty()){
        qDebug() << "No access token!";
        return;
    }
    else if (pathToPic.isEmpty()){
        qDebug() << "No path to file!";
        return;
    }
    else if (albumID.isEmpty()){
        qDebug() << "No target upload album ID!";
        return;
     }

    /* Disconnect all previous connection to avoid multiple trigger */
    this->disconnect();
    /* Start uploading */
    UploadPicData(pathToPic);
    /* Create the file on Google Photo */
    connect(this,SIGNAL(uploadTokenReceived(QString const)),this,SLOT(CreateMediaInAlbum(QString const)));
}



void GooglePhoto::UploadPicData(QString const &path){
        qDebug() << "Uploading binary";

        if (manager == nullptr) {
             manager = new QNetworkAccessManager(this);
         }
        /* set to true at the beginning of every upload, prevent parallel uploads */
        Uploading = true;

        /* Read binary data of the file */
        QFileInfo info(path);
        QFile file(path);
        fileName  = file.fileName();

        file.open(QIODevice::ReadOnly);
        QByteArray fileBytes = file.readAll();
        file.close();

        /* build the API request */
        QNetworkRequest req (QUrl("https://photoslibrary.googleapis.com/v1/uploads"));
        req.setRawHeader("Authorization","Bearer "+ accessToken.toUtf8());
        req.setRawHeader("Content-Type","application/octet-stream");
        req.setRawHeader("X-Goog-Upload-File-Name",info.fileName().toLocal8Bit());
        req.setRawHeader("X-Goog-Upload-Protocol", "raw");

        manager->post(req, fileBytes);
        connect(this->manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(UploadReply(QNetworkReply*)));
}

void GooglePhoto::UploadReply(QNetworkReply *reply) {
    if(reply->error()) {
        qDebug() << "Upload Binary Data Error!";
        QString err = reply->errorString();
        Uploading = false;
        manager->disconnect();
        emit mediaCreateFailed(fileName);

        if (err == "Host accounts.google.com not found")
            emit showMessage(err + QString(". Check WiFi connection"));
        else
            emit showMessage(err);

    } else {
        qDebug() << "Upload Binary Data Success!";
        uploadToken = QString(reply->readAll());
        manager->disconnect();
        emit uploadTokenReceived(uploadToken);
     }
}


void GooglePhoto::CreateMediaInAlbum(QString const &token){
    qDebug() << "Creating media in Album";
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }

    QUrl endpoint("https://photoslibrary.googleapis.com/v1/mediaItems:batchCreate");
    QNetworkRequest req(endpoint);
    req.setRawHeader("Content-Type","application/json");
    req.setRawHeader("Authorization","Bearer "+ accessToken.toUtf8());

    QJsonObject temp;
        temp["uploadToken"] = token;

    if(albumDescription.isEmpty()){
        albumDescription = QString("Not available");
     }
    QJsonObject temp2;
    temp2 [ "description" ] = albumDescription;
    temp2 ["simpleMediaItem"] = temp;
    QJsonArray arr;
    arr.append(temp2);
    QJsonObject obj;
    obj ["newMediaItems"] = arr;
    obj ["albumId"] = albumID;

    QJsonDocument doc (obj);
    QByteArray jsonRequest = doc.toJson(QJsonDocument::Compact);
    QByteArray postDataSize = QByteArray::number(jsonRequest.size());
    req.setRawHeader("Content-Length", postDataSize);
    manager->post(req,jsonRequest);
    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(CreateMediaReply(QNetworkReply*)));
}

void GooglePhoto::CreateMediaReply(QNetworkReply *reply) {
    if(reply->error()) {
        qDebug() << "Create Media Error" << reply->readAll();
        manager->disconnect();
        emit mediaCreateFailed(fileName);

    } else {
        qDebug() << "Create Media Success!";
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObj = jsonDoc.object();
        uploadedPhotoURL = jsonObj["newMediaItemResults"].toArray()[0].toObject()["mediaItem"].toObject()["productUrl"].toString();
        manager->disconnect();
        manager->disconnect();
        emit mediaCreated(fileName);

    }
    /* set flag to false wheter upload is success or not */
    Uploading = false;
}

void GooglePhoto::CreateAlbum(){
    qDebug() << "Creating new album!";
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }

    QUrl endpoint("https://photoslibrary.googleapis.com/v1/albums");
    QNetworkRequest req(endpoint);
    req.setRawHeader("Content-Type","application/json");
    req.setRawHeader("Authorization","Bearer "+ accessToken.toUtf8());

    QJsonObject obj;
    obj["title"] = albumName;
    QJsonObject jsonObj {
        {"album",obj}
    };

    QJsonDocument doc (jsonObj);
    QByteArray jsonRequest = doc.toJson(QJsonDocument::Compact);
    QByteArray postDataSize = QByteArray::number(jsonRequest.size());
    req.setRawHeader("Content-Length", postDataSize);

    manager->post(req,jsonRequest);
    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(CreateAlbumReply(QNetworkReply*)));
}


void GooglePhoto::CreateAlbumReply(QNetworkReply * reply){
    if(reply->error()) {
        qDebug() << "Create Album Error" << reply->readAll();
        manager->disconnect();
    } else {
        qDebug() << "Create Album Success!";
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObj = jsonDoc.object();
        albumID = jsonObj["id"].toString();
//        qDebug() << "Album created! ID:" << albumID;
        albumURL = jsonObj["productUrl"].toString();
//        qDebug() << "Album link:" << albumURL;
        manager->disconnect();
        emit albumCreated();
        emit albumIdChanged(albumID,albumName);
        emit showMessage("Album created successfully");
     }
}

void GooglePhoto::ShareAlbum(){
    if (manager == nullptr) {
         manager = new QNetworkAccessManager(this);
     }

    QString endpoint ("https://photoslibrary.googleapis.com/v1/albums/");
    QUrl reqURL(endpoint + albumID + QString(":share"));
    QNetworkRequest req(reqURL);
    req.setRawHeader("Authorization","Bearer "+ accessToken.toUtf8());
    req.setRawHeader("Content-Type","application/json");
    QJsonObject temp{
        {"isCollaborative", "false"},
        {"isCommentable", "false"}
    };
    QJsonObject jsonObj{
        {"sharedAlbumOptions", temp}
    };
    QJsonDocument doc (jsonObj);
    QByteArray jsonRequest = doc.toJson(QJsonDocument::Compact);
    QByteArray postDataSize = QByteArray::number(jsonRequest.size());
    req.setRawHeader("Content-Length", postDataSize);
    manager->post(req,jsonRequest);
    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(ShareAlbumReply(QNetworkReply*)));
}
void GooglePhoto::ShareAlbumReply(QNetworkReply * reply){
    if(reply->error()) {
        qDebug() << "Sharing Albums Error" << reply->readAll();
        manager->disconnect();
    } else {
        qDebug() << "Sharing Albums Success";
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObj = jsonDoc.object();
        albumURL =  jsonObj["shareInfo"].toObject()["shareableUrl"].toString();
        albumReady = true;
        manager->disconnect();
        emit albumShared(albumURL);
     }

}
/* Must use non-sharing scope when request OAuth2 for those functions */
void GooglePhoto::GetAlbums(){
    if (manager == nullptr) {
        manager = new QNetworkAccessManager(this);
    }
    QUrl endpoint("https://photoslibrary.googleapis.com/v1/albums");
    QNetworkRequest req(endpoint);
    req.setRawHeader("Authorization","Bearer "+ accessToken.toUtf8());
    manager->get(req);
    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(GetAlbumsReply(QNetworkReply*)));
}

void GooglePhoto::GetAlbumsReply(QNetworkReply * reply){
    if(reply->error()) {
        qDebug() << "Get Shared Albums Error" << reply->readAll();
    } else {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObj = jsonDoc.object();
//        qDebug() << jsonObj;
     }
    manager->disconnect();
}

void GooglePhoto::Reauthenticate(){
    auth->Authenticate();   //Share scope cannot querry for list of albums from Google Photo
    connect(auth,SIGNAL(tokenReady(QString const)),this,SLOT(SetAccessToken(QString const)));

}

QString GooglePhoto::GetAlbumID(){
    return albumID;
}

QString GooglePhoto::GetAlbumName(){
    return albumName;
}

QString GooglePhoto::GetAlbumURL(){
    return albumURL;
}

QString GooglePhoto::GetUploadedPhotoURL(){
    return uploadedPhotoURL;
}


GooglePhoto::~GooglePhoto(){

}
