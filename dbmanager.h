#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlRecord>

#include <QObject>
#include <QDateTime>
class DBmanager : public QObject
{
    Q_OBJECT
public:
    DBmanager(QString const &connectionName, QString const &db_path = "");
    ~DBmanager();

public slots:
    void closeConnection(QString const &connectionName);
    void setDatabasePath(QString const &filePath);
    void initializeAllTable();
    int getMaxRowCount();

    /* photo */
    void initializePhotoTable();
    void connectPhotoTable();
    void addPhoto(QString const &filename,
                  QString const &album,
                  QString const &status,
                  QString const &dateAdded,
                  QString const &dateModified,
                  QString const &path);
    void clearPhoto();
    void removePhoto(int row);
    void setPhotoStatus(int row, QString const &status);
    void setAlbumName(int row, QString const &album);
    void setPhotoStatusByPath(QString const &path, QString const &status);
    QSqlTableModel *getPhotoTable();

    /* watched folder */
    void initializeWatchedTable();
    void connectWatchedTable();
    void addWatched(QString const &folderName,
                    QString const &status,
                    int num_file,
                    QString const &dateAdded,
                    QString const &dateModified,
                    QString const &path);
    void clearWatched();
    void removeWatched(int row);
    void setWatchedStatus(int row, QString const &status);
    void setWatchedNumFile(int row, int num_file);
    QSqlTableModel *getWatchedTable();

    /* email */
    void initializeEmailTable();
    void connectEmailTable();
    void clearEmail();
    void removeEmail(int row);
    void addEmail(QString const &email,
                  QString const &status,
                  int num_file,
                  QString const &dateAdded,
                  QString const &path);
    bool emailExists(QString const &email,
                 QString const &path);
    void setStatusEmail(int row, QString const &status);
    QSqlTableModel *getEmailTable();


    /* sms */
    void initializeSmsTable();
    void connectSMSTable();
    void addSMS(QString const &phone,
                  QString const &carrier,
                  QString const &status,
                  int num_file,
                  QString const &dateAdded,
                  QString const &path);
    bool smsExists(QString const &phone,
                               QString const &carrier,
                               QString const &path);
    void clearSMS();
    void removeSMS(int row);
    void setStatusSMS(int row, QString const &status);
    QSqlTableModel *getSMSTable();

private:
    QSqlDatabase m_db;
    QSqlTableModel *smsTable = nullptr;
    QSqlTableModel *emailTable = nullptr;
    QSqlTableModel *photoTable = nullptr;
    QSqlTableModel *watchedTable = nullptr;


signals:
    void finished(); // Connect signal to refresh view
    void showMessage(QString const &msg);
    void initAllTableDone();
};

#endif // DBMANAGER_H
