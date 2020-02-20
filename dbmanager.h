#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>

#include <QObject>
#include <QDateTime>
class DBmanager : public QObject
{
    Q_OBJECT
public:
    explicit DBmanager(QObject *parent = nullptr);
    DBmanager();

public slots:
    bool isEmpty();
//    void addPhoto();
//    void removePhoto();
//    bool photoExists();
    void createPhotoTable();
    void createWatchFolderTable();

    void createEmailTable();
    void addEmail(QString const &email,
                  QString const &status,
                  int num_file,
                  QString const &dateAdded,
                  QString const &path);
    void clearEmail();
    void connectEmailTable();
    void removeEmail(int row);
    void setStatusEmail(int row, QString const &status);
    QSqlTableModel *getEmailTable();

    void createSmsTable();
    void addSMS(QString const &phone,
                  QString const &carrier,
                  QString const &status,
                  int num_file,
                  QString const &dateAdded,
                  QString const &path);
    void clearSMS();
    void connectSMSTable();
    void removeSMS(int row);
    void setStatusSMS(int row, QString const &status);
    QSqlTableModel *getSMSTable();

private:
    QSqlDatabase m_db;
    QSqlTableModel *smsTable = nullptr;
    QSqlTableModel *emailTable = nullptr;


signals:

};

#endif // DBMANAGER_H
