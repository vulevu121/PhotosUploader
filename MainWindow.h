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
#include "SettingsDialog.h"
#include "CreateAlbumDialog.h"
#include "EmailTemplateDialog.h"
#include "SMSTemplateDialog.h"
#include "googlephoto.h"
#include "gmail.h"
#include "googleoauth2.h"

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
    void addFolder();
    void removeFolders();
    void clearWatchlist();
    void createAlbum(QString const &name, QString const &desc, QString const &albumId, bool newExistingAlbum);

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
    void sendNow(QString const &to, QString const &subject, QString const &body);
    void updateFailedList(QString const &filename);

    void syncSettings();
    void showEmailTemplate();
    void showSMSTemplate();

    void resumeQueue();
    void stopQueue();

    void resetFailItems();
    void emailOut();
    void emailInit();

    void googleLogIn();
    void googleLogOut();

    void deleteAllObjects();

    QString getAlbumIdFromFile();

public slots:
    void logInit();
    void importLog();
    void saveLog();
    void saveAlbumId(QString const &id);


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
    QTimer *  saveTimer = nullptr ;
    QTimer elapsedTime;

    QStringList uploadedList;
    QJsonArray uploadedListJson;
    QMap<QString,int> uploadFailedList;
    bool isReady = true;
    QJsonObject jsonObj;
    QString logPath;


    QSettings *settings = new QSettings("Pixyl", "PixylPush");
};

#endif // MAINWINDOW_H
