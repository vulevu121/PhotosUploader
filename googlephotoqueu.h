#ifndef GOOGLEPHOTOQUEU_H
#define GOOGLEPHOTOQUEU_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QUrl>
#include <QTimer>
#include <QThread>
#include <QFileSystemWatcher>
#include <QDir>
#include "gmail.h"
#include "googlephoto.h"

class GooglePhotoQueu: public QObject
{
    Q_OBJECT
public:
    explicit GooglePhotoQueu(QObject *parent = nullptr);

    GMAIL * email = nullptr;
    GooglePhoto  *p = nullptr;
    QTimer * timer1 = nullptr;
    QTimer * timer2 = nullptr;
    QJsonObject object ;
    QStringList uploadedList;
    QStringList uploadList;
    QFileSystemWatcher * camera_folder_watcher = nullptr;
    QFileSystemWatcher * template_folder_watcher = nullptr;
    bool isReady = true;
    QString pathToLog = "C:/Users/khuon/Documents/GooglePhoto/Upload Log.json";


public slots:
    void CheckCameraFolder();
    void CheckUploadList();
    void UpdateUploadedList(QString);
    void OpenLog();
    void CloseLog();
    void begin();

private:
    QString camera_folder_path  = QString("C:/Users/khuon/Documents/GooglePhoto/camera");
    QDir * camera_folder;
    QStringList images;

    QString template_folder_path = QString("C:/Users/khuon/Documents/GooglePhoto/template");
    QDir * template_folder;

    QString uploaded_folder_path = QString("C:/Users/khuon/Documents/GooglePhoto/uploaded");
    QDir * uploaded_folder;


};

#endif // GOOGLEPHOTOQUEU_H
