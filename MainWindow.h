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
    EmailWorker();
    ~EmailWorker();

public slots:
    void sendSMTP(QString const &sender,
             QString const &receiver,
             QString const &sub,
             QString const &body,
             QStringList paths,
             QString const &albumName,
             QString const &albumUrl);

signals:
    void finished();

private:
    QSettings *settings = new QSettings("Pixyl", "PixylPush");
    QString body;
    QString subject;
    QString to;
    QString from;
    QString timeFormat = "MM/dd/yyyy hh:mm AP";

private slots:

};



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
    void addToQueueModel(QList<QStandardItem *> queueRow);
    void removeQueues();
    void clearQueue();
    void addFolder(QString const &folderPath);
    void addFolders();
    void removeFolders();
    void clearWatchlist();
    void createAlbum(QString const &name, QString const &desc);
    void linkExistingAlbum(QString const &id);
    void showCreateAlbumDialog();
    void queueUpload();
    void scaleImage(QString const &filePath);
    void scaleImages(QDir &dir);

    void folderScan();
    void queueTimerInit();
    void queueTimerStart();
    void queueTimerStop();
    void progressBarTimerInit();
    void progressBarUpdate();
    void folderTimerInit();
    void folderTimerStart();
    void folderTimerStop();
    void saveTimerInit();

    void updateUploadedList(QString const &filePath);
    void updateFailedList(QString const &filePath);
    void syncSettings();
    void showEmailTemplate();
    void showSMSTemplate();
    void resumeQueue();
    void stopQueue();
    void resetFailedUpload();
    bool sendSMTP(QString const &sender,
                  QString const &receiver,
                  QString const &sub,
                  QString const &body,
                  QStringList paths);

    bool sendSMTPsms( QString const &receiver,
                      QString const &guest_num,
                      QString const &body,
                      QStringList paths);

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
    bool compareStringList(QStringList &a, QStringList &b);

    /* Email */
    void addEmailQueue();
    void removeEmailQueue();
    void clearEmailQueue();
    void addToWatchModel(QList<QStandardItem *> watchRow);
//    void exportEmailLog();
    void importToEmailModel(const QString &emailPath);
    void emailGuests();
    void selectFileToEmail();
    void addEmailRow();

    /* SMS */
    void addSMSQueue();
    void selectSMSFile();
    void addSMSRow();
    void removeSMSQueue();
    void clearSMSQueue();
    void importToSMSModel(const QString &smsPath);
    void smsGuests();
//    void exportSMSLog();

    void showErrMsg();
    void enableAddButtons();
    void disableAddButtons();
    void importMastertLog();
    void importUploadLog(QDir dir);
    void logToModel();
    void scanDirectory(QString const &path);
    void importLastScannedFolders();
    void saveUploadLog();
    void saveMasterLog();
    void saveUsedAlbum(QString const &id, QString const &name);
    QString loadUsedAlbum(QString const &key);
    QJsonArray openFile(const QString &path);
    QStringList toStringList(QJsonArray &array);
    void updateProgressBar(int val);

public slots:
    QIcon colorIcon(const QString &path, const QColor &color);

private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog = nullptr;
    QStandardItemModel *queueModel = nullptr;
    QStringList queueHeader;
    QStandardItemModel *watchModel = nullptr;
    QStringList watchHeader;
    QStandardItemModel * emailModel = nullptr;
    QStringList emailHeader;
    QStandardItemModel * smsModel = nullptr;
    QStringList smsHeader;
    CreateAlbumDialog * createAlbumDialog = nullptr;
    GooglePhoto * gphoto = nullptr;
    GMAIL *email = nullptr;
    GoogleOAuth2 * auth = nullptr;
    QTimer * queueTimer = nullptr;
    QTimer * folderTimer = nullptr;
    QTimer * saveTimer = nullptr ;
    QTimer * progressBarTimer = nullptr;
    QTimer elapsedTime;
    QStringList uploadedList;
    QJsonArray uploadedListJson;
    QMap<QString,int> uploadFailedList;
    bool isReady = true;
    EmailTemplateDialog * emailDialog = nullptr;
    SMSTemplateDialog  * smsDialog = nullptr;
    QFileSystemWatcher * watcher = nullptr;

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


class Worker : public QObject {
    Q_OBJECT
public:
    Worker();
    ~Worker();

public slots:
    void setupQueue(QStandardItemModel* model,QStringList const &header);
    void setupWatch(QStandardItemModel* model, QStringList const &header);
    void process();
    void setIntervalQueue(int interval);
    void setIntervalWatch(int interval);


signals:
    void finished();
    void error(QString err);

private:
    QTimer * m_queueTimer = nullptr;
    QTimer * m_watchTimer = nullptr;
    QStandardItemModel *w_queueModel = nullptr;
    QStringList w_queueHeader;
    QStandardItemModel *w_watchModel = nullptr;
    QStringList w_watchHeader;
    QString timeFormat = "MM/dd/yyyy hh:mm AP";
    QSettings *settings = new QSettings("Pixyl", "PixylPush");

private slots:
    void startTimer();
    void stopTimer();
    void processQueue();
    void processWatch();
    void w_addToQueueModel(QList<QStandardItem *> row);

};




#endif // MAINWINDOW_H
