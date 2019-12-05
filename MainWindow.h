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

#include "SettingsDialog.h"
#include "CreateAlbumDialog.h"
#include "EmailTemplateDialog.h"
#include "SMSTemplateDialog.h"
#include "googlephoto.h"
#include "gmail.h"
#include "googleoauth2.h"
#include "filedownloader.h"
#include "C:/SmtpClient-for-Qt-1.1/src/SmtpMime"

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

    void updateUploadedList(QString const &filename);
    void updateFailedList(QString const &filename);

    void syncSettings();
    void showEmailTemplate();
    void showSMSTemplate();

    void resumeQueue();
    void stopQueue();

    void resetFailItems();
    bool sendSMTP(QString const &sender,
                  QString const &receiver,
                  QString const &sub,
                  QString const &body,
                  QStringList const &paths);

    bool sendSMTPsms( QString const &receiver,
                      QString const &guest_num,
                      QString const &body,
                      QStringList const &paths);

    void googleLogIn();
    void googleLogOut();

    void deleteAllObjects();

    void disableCreateAlbumBtn();
    void enableCreateAlbumBtn(QString const &blank ="");
    void disableLogInBtn();
    void enableLogInBtn(QString const &blank = "");
    void disableLogOutBtn();
    void enableLogOutBtn(QString const &blank = "");
    void displayAlbumName(QString const &id, QString const &name);

    void downloadQR(QString const &url="www.google.com");
    void saveQR(QString const &location);
    void prepQrLocation();
    bool compareStringList(QStringList &a, QStringList &b);

    void addEmailQueue();
    void removeEmailQueue();
    void clearEmailQueue();
    void addToEmailModel(QList<QStandardItem*> line);

    void emailGuests();

    void addSMSQueue(QList<QStandardItem*> line);
    void smsGuests();

    void showErrMsg();
    void enableAddButtons();
    void disableAddButtons();

public slots:
    void importMastertLog();
    void findUploadLog(QString const &folder_path);
    void importLastScannedFolders();
    void saveProgress();
    void saveMasterLog();
    void saveUsedAlbum(QString const &id, QString const &name);
    QIcon colorIcon(const QString &path, const QColor &color);
    QString loadUsedAlbum(QString const &key);

    void importToEmailModel(const QString &emailPath);
    QJsonArray openFile(const QString &path);
    QStringList toStringList(QJsonArray &array, QString key);

    void importSMSQueue(const QString &smsPath);
    void updateProgressBar(int val);

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
    QJsonObject jsonObj;

    EmailTemplateDialog *emailDialog = nullptr;
    SMSTemplateDialog *smsDialog = nullptr;
    QFileSystemWatcher * emailWatcher = nullptr;
    QFileSystemWatcher * smsWatcher = nullptr;
    QMap<QString,QString> carrier_map;
    FileDownloader *m_pImgCtrl = nullptr;
    QMessageBox msgBx;


    QColor grey = QColor("grey");
    QColor white = QColor("white");
    QString timeFormat = "MM/dd/yyyy hh:mm AP";
    QSettings *settings = new QSettings("Pixyl", "PixylPush");
};

#endif // MAINWINDOW_H
