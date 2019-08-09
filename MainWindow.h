#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include "SettingsDialog.h"
#include "googlephotoqueu.h"
#include "CreateAlbumDialog.h"
#include "EmailTemplateDialog.h"
#include "SMSTemplateDialog.h"

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
//    void createAlbumSignal(QString const &name, QString const &desc, QString const &albumId, bool newExistingAlbum);
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

    void updateUploadedList(QString);
    void sendNow(QString const &to, QString const &subject, QString const &body);
    void saveLog();

    void syncSettings();
    void showEmailTemplate();
    void showSMSTemplate();

private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog = nullptr;
    QStandardItemModel *queueModel = nullptr;
    QStringList queueHeader;

    QStandardItemModel *watchModel = nullptr;
    QStringList watchHeader;

    CreateAlbumDialog * createAlbumDialog = nullptr;
    GooglePhoto * gphoto = nullptr;
    QTimer * queueTimer = nullptr;
    QTimer * folderTimer = nullptr;
    QStringList uploadedList;
    QJsonArray uploadedListJson;
    bool isReady = true;
    GMAIL *email = nullptr;



    QSettings *settings = new QSettings("Pixyl", "PixylLibrary");
};

#endif // MAINWINDOW_H
