#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QList>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QBitmap>
#include <QPixmap>
#include <QIcon>
#include <QColor>
#include <QFileSystemWatcher>
#include <QTime>
#include <QMessageBox>
#include <QThread>
#include <QProgressBar>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>

#include "SettingsDialog.h"
#include "CreateAlbumDialog.h"
#include "EmailTemplateDialog.h"
#include "SMSTemplateDialog.h"
#include "googlephoto.h"
#include "gmail.h"
#include "googleoauth2.h"
#include "filedownloader.h"
#include "dbmanager.h"
#include "C:/SmtpClient-for-Qt-1.1/src/SmtpMime"


class EmailWorker : public QObject {
    Q_OBJECT
public:
    EmailWorker(DBmanager *db, GooglePhoto *gphoto);
    ~EmailWorker();

public slots:
    void sendSMTP(QString const &sender,
                          QString const &receiver,
                          QString const &sub,
                          QString const &body,
                          QStringList paths);
    void emailGuests();

signals:
    void finished();

private:
    DBmanager *m_db;
    GooglePhoto *m_gphoto;
    QSettings *settings = new QSettings("Pixyl", "PixylPush");
    QString body;
    QString subject;
    QString TO;
    QString from;
    QString timeFormat = "MM/dd/yyyy hh:mm AP";

private slots:

};


/********** WORKER ************/
class Worker : public QObject {
    Q_OBJECT
public:
    Worker();
    ~Worker();

public slots:
    void work();
    void stopWork();

signals:
    void progress(int);

private:
    bool stop = false;

};
/*******************************/


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void existingAlbumSignal(QString const &id);
    void folderAdded (QString const &folderPath);

private slots:
    void addQueue();
    void removeQueue();
    void clearQueue();
    void addFolder(QString const &folderPath);
    void addFolders();
    void removeFolder();
    void clearFolders();
    void createAlbum(QString const &name, QString const &desc);
    void linkExistingAlbum(QString const &id);
    void showCreateAlbumDialog();
    void uploadQueue();
    void scaleImage(QString const &filePath);
    void scaleImages(QDir &dir);

    void scanFolder();
    void queueTimerInit();
    void queueTimerStart();
    void queueTimerStop();
    void folderTimerInit();
    void folderTimerStart();
    void folderTimerStop();
    void saveTimerInit();

    void setQueueStatusComplete(QString const &filePath);
    void setQueueStatusFailed(QString const &filePath);
    void syncSettings();
    void showEmailTemplate();
    void showSMSTemplate();
    void resumeQueue();
    void stopQueue();
    void resetQueueStatus();

    void googleLogIn();
    void googleLogOut();
    void deleteAllObjects();
    void disableCreateAlbumBtn();
    void enableCreateAlbumBtn();
    void disableLogInBtn();
    void enableLogInBtn();
    void disableLogOutBtn();
    void enableLogOutBtn();
    void displayAlbumName(QString const &id, QString const &name);
    void downloadQR(QString const &url="www.google.com");
    void saveQR(QString const &location);
    void prepQrLocation();
    void showProgressBar();
    void hideProgressBar();

    /* Email */
    void addEmailQueue();
    void removeEmailQueue();
    void clearEmailQueue();
//    void exportEmailLog();
    void importToEmailModel(const QString &emailPath);
    void emailGuests();
    void selectFileToEmail();
    void addEmailRow();
    bool sendSMTP(QString const &sender,
                  QString const &receiver,
                  QString const &sub,
                  QString const &body,
                  QStringList paths);

    /* SMS */
    void addSMSQueue();
    void selectSMSFile();
    void addSMSRow();
    void removeSMSQueue();
    void clearSMSQueue();
    void importToSMSModel(const QString &smsPath);
    void smsGuests();
//    void exportSMSLog();
    bool sendSMTPsms( QString const &receiver,
                      QString const &guest_num,
                      QString const &body,
                      QStringList paths);

    void showErrMsg();
    void enableResume();
    void enableStop();
    void disableResume();
    void disableStop();
    void enableAddButtons();
    void disableAddButtons();
    void scanDirectory(QString const &path);

    QString loadUsedAlbum(QString const &key);
    QJsonArray openFile(const QString &path);
    QStringList toStringList(QJsonArray &array);

public slots:
    QIcon colorIcon(const QString &path, const QColor &color);

private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog = nullptr;
    CreateAlbumDialog * createAlbumDialog = nullptr;
    GooglePhoto * gphoto = nullptr;
    GMAIL *email = nullptr;
    GoogleOAuth2 * auth = nullptr;
    QTimer * queueTimer = nullptr;
    QTimer * folderTimer = nullptr;
    QTimer * saveTimer = nullptr ;
    QTimer elapsedTime;
//    QStringList uploadedList;
//    QJsonArray uploadedListJson;

    QMap<QString,int> uploadFailedList;
    bool isReady = true;
    EmailTemplateDialog * emailDialog = nullptr;
    SMSTemplateDialog  * smsDialog = nullptr;
    QFileSystemWatcher * watcher = nullptr;

    QThread* thread = nullptr;
    Worker* worker = nullptr;
    bool isDone = true;
    QString const SMTP_user = "info.enchanted.oc@gmail.com";
    QString const SMTP_pass = "PixylBoys$2020";
    QMap<QString,QString> carrier_map;
    FileDownloader *m_pImgCtrl = nullptr;
    QMessageBox msgBx;
    QString albumFolderPath;
    QColor grey = QColor("grey");
    QColor white = QColor("white");
    QString timeFormat = "MM/dd/yyyy hh:mm AP";
    QSettings *settings = new QSettings("Pixyl", "PixylPush");

    DBmanager *m_db = nullptr;

};





#endif // MAINWINDOW_H
