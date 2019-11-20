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

#include "SettingsDialog.h"
#include "CreateAlbumDialog.h"
#include "EmailTemplateDialog.h"
#include "SMSTemplateDialog.h"
#include "googlephoto.h"
#include "gmail.h"
#include "googleoauth2.h"
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


    void sendSMTP(QString const &sender,
                  QString const &receiver,
                  QString const &sub,
                  QString const &body,
                  QStringList const &paths);

    void sendSMTPsms( QString const &receiver,
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


public slots:
    void importMastertLog();
    void searchFolderLog(QString const &folder_path);
    void importLastScannedFolders();
    void saveProgress();
    void saveMasterLog();
    void saveUsedAlbum(QString const &id, QString const &name);
    QIcon colorIcon(const QString &path, const QColor &color);
    QString loadUsedAlbum(QString const &key);

    void importEmailQueue(const QString &emailPath);
    void importSMSQueue(const QString &smsPath);


private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog = nullptr;
    QStandardItemModel *queueModel = nullptr;
    QStringList queueHeader;

    QStandardItemModel *watchModel = nullptr;
    QStringList watchHeader;

    CreateAlbumDialog * createAlbumDialog = nullptr;
    GooglePhoto * gphoto = nullptr;
    GMAIL *email = nullptr;
    GoogleOAuth2 * auth = nullptr;

    QTimer * queueTimer = nullptr;
    QTimer * folderTimer = nullptr;
    QTimer * saveTimer = nullptr ;
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

    QColor grey = QColor("grey");
    QColor white = QColor("white");
    QString timeFormat = "MM/dd/yyyy hh:mm AP";
    QSettings *settings = new QSettings("Pixyl", "PixylPush");
};

#endif // MAINWINDOW_H
