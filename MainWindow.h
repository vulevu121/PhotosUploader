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
#include <QPropertyAnimation>
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


/******* EMAIL WORKER ********/
class EmailWorker : public QObject {
    Q_OBJECT
public:
    EmailWorker(DBmanager *db,GooglePhoto *gphoto, QString const &user, QString const &password);
    ~EmailWorker();

private:
    DBmanager *m_db = nullptr;
    GooglePhoto *m_gphoto;
    QSettings *settings = new QSettings("Pixyl", "PixylPush");
    QString m_body;
    QString m_subject;
    QString m_to;
    QString m_from;
    QString timeFormat = "MM/dd/yyyy hh:mm AP";
    QString m_SMTP_user;
    QString m_SMTP_pass;
    QString connection_name = "thread1_connection";

signals:
    void updatedRow(int row);
    void finished();

public slots:
    void processQueue();
    bool sendSMTP(QString const &sender,
                          QString const &receiver,
                          QString const &sub,
                          QString const &body,
                          QStringList paths);
};

/******** SMS WORKER ******/
class SMSWorker : public QObject {
    Q_OBJECT
public:
    SMSWorker(DBmanager *db, GooglePhoto *gphoto, QString const &user, QString const &password,QMap<QString,QString> map);
    ~SMSWorker();

private:
    DBmanager *m_db = nullptr;
    GooglePhoto *m_gphoto;
    QSettings *settings = new QSettings("Pixyl", "PixylPush");
    QString m_body;
    QString m_from;
    QString timeFormat = "MM/dd/yyyy hh:mm AP";
    QString m_SMTP_user;
    QString m_SMTP_pass;
    QMap<QString,QString> m_map;

signals:
    void updatedRow(int row);
    void finished();

public slots:
    void processQueue();
    bool sendSMTP(QString const &receiver,
                  QString const &guest_num,
                  QString const &body,
                  QStringList paths);
};




/********** PROGRESS BAR WORKER ************/
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
    void uploadQueue();
    void setQueueStatusComplete(QString const &filePath);
    void setQueueStatusFailed(QString const &filePath);
    void resumeQueue();
    void stopQueue();
    void resetQueueStatus();

    void addFolder(QString const &folderPath);
    void addFolderRequest();
    void removeFolder();
    void clearFolders();
    void scanFolder();
    void scanTxtFiles(QString const &folderPath);

    /* Album methods */
    void createAlbum(QString const &name, QString const &desc);
    void linkExistingAlbum(QString const &id);
    void showCreateAlbumDialog();
    void displayAlbumName(QString const &id, QString const &name);
    QString loadUsedAlbum(QString const &key);
    QJsonArray openFile(const QString &path);
    QStringList toStringList(QJsonArray &array);

    /* Scale image */
    int scaleImage(QString const &filePath); //return size of scaled image
    void scaleImages(QDir &dir);

    /* Timer */
    void queueTimerInit();
    void queueTimerStart();
    void queueTimerStop();
    void folderTimerInit();
    void folderTimerStart();
    void folderTimerStop();
    void saveTimerInit();

    /* QR Code */
    void downloadQR(QString const &url="www.google.com");
    void saveQR(QString const &location);
    void prepQrLocation();


    /* Email */
    void showEmailTemplate();
    void addEmailQueue();
    void removeEmailQueue();
    void clearEmailQueue();
    void importToEmailModel(const QString &emailPath);
    void initializeThreadEmail();
    void selectFileToEmail();
    void addUserInputEmailQueue();
    void cancelUserInputEmailQueue();
    void updateEmailView(int row);
//    void exportEmailLog();


    /* SMS */
    void showSMSTemplate();
    void addSMSQueue();
    void selectFileToSMS();
    void addUserInputSMSQueue();
    void cancelUserInputSMSQueue();
    void removeSMSQueue();
    void clearSMSQueue();
    void importToSMSModel(const QString &smsPath);
    void initializeThreadSMS();
    void updateSMSView(int row);
//    void exportSMSLog();


    /* Log In - Log Out */
    void googleLogIn();
    void googleLogOut();

    /* Enable Buttons */
    void enableCreateAlbumBtn();
    void enableLogInBtn();
    void enableLogOutBtn();
    void enableResume();
    void enableStop();
    void enableAddButtons();

    /* Disable Button */
    void disableCreateAlbumBtn();
    void disableLogInBtn();
    void disableLogOutBtn();
    void disableResume();
    void disableStop();
    void disableAddButtons();

    /* Display */
    void showErrMsg();
    bool promptLoadNewDatabase();

    /* Database */
    QString getDatabasePath();
    bool saveDatabasePath(QString const &filePath);
    void initializeAllTableView();
    void check4ExistingDatabase(QDir &folder);


    /* Others */
    void syncSettings();
    void deleteAllObjects();
    int getTotalSize(QStringList &filePaths);
    void startThread();
    void showThreadDone();
    void showProgressBar();
    void hideProgressBar();
    void checkTableStatus();

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

    QMap<QString,int> uploadFailedList;
    bool isReady = true;
    EmailTemplateDialog * emailDialog = nullptr;
    SMSTemplateDialog  * smsDialog = nullptr;
    QFileSystemWatcher  watcher;

    QThread *thread_email = nullptr;
    QThread *thread_sms = nullptr;


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
    Worker *worker;
    EmailWorker *email_worker = nullptr;
    SMSWorker *sms_worker = nullptr;
    QPropertyAnimation *progressBarAnimate = nullptr;

};


#endif // MAINWINDOW_H
