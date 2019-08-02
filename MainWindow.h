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
    void createAlbum(QString const &name, QString const &desc);
    void showCreateAlbumDialog();

private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog = nullptr;
    QStandardItemModel *queueModel = nullptr;
    QStandardItemModel *watchModel = nullptr;


};

#endif // MAINWINDOW_H
