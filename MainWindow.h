#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include "SettingsDialog.h"
#include "googlephotoqueu.h"
#include "CreateAlbumDialog.h"

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
    void createAlbumSignal(QString const &name, QString const &desc);

private slots:
    void addQueue();
    void removeQueues();
    void clearQueue();
    void addFolder();
    void removeFolders();
    void clearWatchlist();
//    void createAlbum(QString const &name, QString const &desc);
    void createAlbum(QString const &name, QString const &desc);

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
    void emailLink(QString const &to, QString const &subject, QString const &body);

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

};

#endif // MAINWINDOW_H
