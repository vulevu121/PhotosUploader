#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "SettingsDialog.h"
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

private slots:
    void addQueue();
    void removeQueues();
    void clearQueue();
    void addFolder();
    void removeFolders();
    void clearWatchlist();
    void createAlbum();

private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog = nullptr;
    QStandardItemModel *queueModel = nullptr;
    QStandardItemModel *watchModel = nullptr;
};

#endif // MAINWINDOW_H
