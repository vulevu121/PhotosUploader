#ifndef GOOGLEPHOTO_H
#define GOOGLEPHOTO_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QUrl>
#include <QFileInfo>
#include "googleoauth2.h"

class GooglePhoto : public QObject
{
    Q_OBJECT
public:
    explicit GooglePhoto(QObject *parent = nullptr);
    ~GooglePhoto();
private:
    QNetworkAccessManager *manager = nullptr;
    GoogleOAuth2 * auth = nullptr ;
    QString accessToken;
    QString uploadToken;
    QString uploadedPhotoURL;
    QString albumName;
    QString albumID;
    QString albumDescription;
    QString albumURL;
    QString pathToFile;
    QString fileName;  //include the absolute path in the file name
    QStringList uploadTokenList;
    bool Uploading = false;
    bool albumReady = false;


signals:
    void authenticated();
    void notAuthenticated(QString const &msg);
    void uploadTokenReceived(QString const &token);
    void albumCreated();
    void albumShared(QString const &url);
    void albumIdChanged(QString const &id);
    void mediaCreated(QString const &filename);
    void mediaCreateFailed(QString const &filename);
    void pathToFileChanged(QString const &newPath);
    void showMessage(QString const &msg);
    void albumIdConnected();

private slots:
    void SetAccessToken(QString const &token);
    void UploadPicData(QString const &path);
    void UploadReply(QNetworkReply *reply);
    void CreateAlbumReply(QNetworkReply * reply);
    void ShareAlbum();
    void ShareAlbumReply(QNetworkReply * reply);
    void CreateMediaInAlbum(QString const &token);
    void CreateMediaReply(QNetworkReply *reply);
    void GetAlbums();
    void GetAlbumsReply(QNetworkReply * reply);
//    void AppendUploadTokenList(QString);
    void GetAlbumById();
    void GetAlbumByIdReply(QNetworkReply * reply);
    void NotAuthenticated(QString const &message);

public slots:
    /* If album already exists, this function will set the target album for all uploads */
    void SetAlbumDescription(QString const &note);
    void SetPathToFile(QString const &path);
    void UploadPhoto(QString const &pathToPic);
    void SetAlbumName(QString const &name);
    bool isUploading();
    bool isAlbumReady();
    void SetTargetAlbumToUpload(QString const &id);
    void CreateAlbum();
    void Reauthenticate();

    bool isAuthenticated();

    QString GetAlbumName();
    QString GetAlbumURL();
    QString GetUploadedPhotoURL();
    /* This requires a different scope. Need to authenticate again*/
    QString GetAlbumID();
};

#endif // GOOGLEPHOTO_H
