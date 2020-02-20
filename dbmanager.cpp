#include "dbmanager.h"
#include <QDebug>

DBmanager::DBmanager(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("C:/Users/khuon/Documents/PixylPushLog/db.sqlite");

    if (!m_db.open()){
        qDebug() << "Error: connection with database fail";
    }
    else{
        qDebug() << "Database: connection ok";
        createSmsTable();
        createEmailTable();
    }
}

//DBmanager::DBmanager(){
//    m_db = QSqlDatabase::addDatabase("QSQLITE");
////    m_db.setDatabaseName(path);
//    m_db.setDatabaseName("C:/Users/khuon/Documents/PixylPushLog/db.sqlite");

//    if (!m_db.open()){
//        qDebug() << "Error: connection with database fail";
//    }
//    else{
//        qDebug() << "Database: connection ok";
//    }
//}
/* return True if db does NOT contain any tables */

bool DBmanager::isEmpty(){
    bool result = true;
    if(m_db.isOpen()){
        if(m_db.tables().length() > 0){
            result = false;

        }
    }
    else
        qDebug() << m_db.lastError();

    return result;
}

/* Create table for photo upload queue */
void DBmanager::createPhotoTable(){
    if(m_db.isOpen()){
        if(!m_db.tables().contains("photos")){
            qDebug() << "create Photo Table in db";
            QSqlQuery query;
            query.exec("create table photos (filename varchar(255) primary key,"
                                                 "album varchar(20),"
                                                 "status varchar(20),"
                                                 "dateAdded DATETIME,"
                                                 "dateModified DATETIME,"
                                                 "path varchar(255))");
            }
//        query.exec("insert into photo (filename, album,status,dateAdded,dateModified,path)"
//                   "values('my photo', 'album','queue','10-20-19','10-21-19','C:/photo')");
    }
}


void DBmanager::createWatchFolderTable(){
    if(m_db.isOpen()){
        if(!m_db.tables().contains("watchedFolders")){
            qDebug() << "create Watched Folder Table  in db";
            QSqlQuery query;
            query.exec("create table watchedFolders (folder varchar(255) primary key,"
                                                 "status varchar(20),"
                                                 "num_files int,"
                                                 "dateAdded DATETIME,"
                                                 "dateModified DATETIME,"
                                                 "path varchar(255))");
        }
//        query.exec("insert into photo (filename, album,status,dateAdded,dateModified,path)"
//                   "values('my photo', 'album','queue','10-20-19','10-21-19','C:/photo')");

    }
}


void DBmanager::createSmsTable(){
    QSqlQuery query;

    if(m_db.isOpen()){
        if(m_db.tables().contains("sms")){
            qDebug() << "SMS table exists.";
            connectSMSTable();
        }else{
            qDebug() << "create SMS Table  in db";
            if(!query.exec("create table sms (phone varchar(255),"
                                                 "carrier varchar(20),"
                                                 "status varchar(20),"
                                                 "num_file int,"
                                                 "dateAdded varchar(255),"
                                                 "path varchar(255),"
                                                 "primary key(phone,carrier,num_file,path))"))
                qDebug() << query.lastError().text();
             else
                connectSMSTable();
        }
    }
}

void DBmanager::connectSMSTable()
{
    if(smsTable == nullptr){
        smsTable = new QSqlTableModel (this,m_db);
        smsTable->setTable("sms");
        smsTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
        smsTable->select();
        qDebug() << "SMS table is linked to sql table model";
    }
}


void DBmanager::addSMS(QString const &phone,
                       QString const &carrier,
                       QString const &status,
                       int num_file,
                       QString const &dateAdded,
                       QString const &path){

    if(smsTable == nullptr){
        qDebug() << " SMS table pointer is NULL";
    }else{
        /* Insert into table, Unique Key prevents duplicate */
        int row = smsTable->rowCount();
        smsTable->insertRows(row,1);
        smsTable->setData((smsTable->index(row,smsTable->fieldIndex("phone"))),phone);
        smsTable->setData((smsTable->index(row,smsTable->fieldIndex("carrier"))),carrier);
        smsTable->setData((smsTable->index(row,smsTable->fieldIndex("status"))),status);
        smsTable->setData((smsTable->index(row,smsTable->fieldIndex("num_file"))),num_file);
        smsTable->setData((smsTable->index(row,smsTable->fieldIndex("dateAdded"))),dateAdded);
        smsTable->setData((smsTable->index(row,smsTable->fieldIndex("path"))),path);
        smsTable->submitAll();
    }
}


void DBmanager::clearSMS(){
    if(smsTable == nullptr){
        qDebug() << " SMS table pointer is NULL";
    }else{
        smsTable->removeRows(0,smsTable->rowCount());
        smsTable->submitAll();
    }
}

void DBmanager::removeSMS(int row){
    if(smsTable == nullptr){
        qDebug() << " SMS table pointer is NULL";
    }else{
        smsTable->removeRow(row);
        smsTable->submitAll();
    }
}


void DBmanager::setStatusSMS(int row, QString const &status){
    if(smsTable == nullptr){
        qDebug() << " SMS table pointer is NULL";
    }else{
        smsTable->setData(smsTable->index(row,smsTable->fieldIndex("status")),status);
        smsTable->submitAll();
    }
}

QSqlTableModel *DBmanager::getSMSTable(){
    if(smsTable == nullptr){
        qDebug() << " SMS table pointer is NULL";
        return nullptr;
    }else{
        return smsTable;
    }
}


void DBmanager::createEmailTable(){
    QSqlQuery query;

    if(m_db.isOpen()){
        if(m_db.tables().contains("email")){
            qDebug() << "email table exists";
            connectEmailTable();
        }
        else
        {
            qDebug() << "create email table in db";
            if(!query.exec("create table email (email varchar(255),"
                                                 "status varchar(20),"
                                                 "num_file int,"
                                                 "dateAdded varchar(255),"
                                                 "path varchar(255),"
                                                 "primary key(email,num_file,path))"))
                qDebug() << query.lastError().text();
            else
                connectEmailTable();
        }
    }
}


void DBmanager::connectEmailTable()
{
    if(emailTable == nullptr){
        emailTable = new QSqlTableModel (this,m_db);
        emailTable->setTable("email");
        emailTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
        emailTable->select();
        qDebug() << "email table is linked to sql table model";
    }
}


void DBmanager::addEmail(QString const &email,
                           QString const &status,
                           int num_file,
                           QString const &dateAdded,
                           QString const &path){

    if(emailTable == nullptr){
        qDebug() << " SMS table pointer is NULL";
    }else{
        /* Insert into table, Unique Key prevents duplicate */
        int row = emailTable->rowCount();
        emailTable->insertRows(row,1);
        emailTable->setData((emailTable->index(row,emailTable->fieldIndex("email"))),email);
        emailTable->setData((emailTable->index(row,emailTable->fieldIndex("status"))),status);
        emailTable->setData((emailTable->index(row,emailTable->fieldIndex("num_file"))),num_file);
        emailTable->setData((emailTable->index(row,emailTable->fieldIndex("dateAdded"))),dateAdded);
        emailTable->setData((emailTable->index(row,emailTable->fieldIndex("path"))),path);
        emailTable->submitAll();
    }
}


void DBmanager::clearEmail(){
    if(emailTable == nullptr){
        qDebug() << " Email table pointer is NULL";
    }else{
        emailTable->removeRows(0,emailTable->rowCount());
        emailTable->submitAll();
    }
}

void DBmanager::removeEmail(int row){
    if(emailTable == nullptr){
        qDebug() << " Email table pointer is NULL";
    }else{
        emailTable->removeRow(row);
        emailTable->submitAll();
    }
}


void DBmanager::setStatusEmail(int row, QString const &status){
    if(emailTable == nullptr){
        qDebug() << " Email table pointer is NULL";
    }else{
        emailTable->setData(emailTable->index(row,emailTable->fieldIndex("status")),status);
        emailTable->submitAll();
    }
}

QSqlTableModel *DBmanager::getEmailTable(){
    if(emailTable == nullptr){
        qDebug() << " Email table pointer is NULL";
        return nullptr;
    }else{
        return emailTable;
    }
}
