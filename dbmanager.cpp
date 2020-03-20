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
        createPhotoTable();
        createWatchedTable();
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

/************************** Photo Table **************************************/
void DBmanager::createPhotoTable(){
    QSqlQuery query;

    if(m_db.isOpen()){
        if(m_db.tables().contains("photo")){
            qDebug() << "Photo table exists.";
            connectPhotoTable();
        }else{
            qDebug() << "create Photo Table  in db";
            if(!query.exec("create table photo (filename varchar(255),"
                                               "album varchar(20),"
                                               "status varchar(20),"
                                               "dateAdded varchar(255),"
                                               "dateModified varchar(255),"
                                               "path varchar(255),"
                                               "primary key(filename,album,path))"))
                qDebug() << query.lastError().text();
             else
                connectPhotoTable();
        }
    }
}
void DBmanager::connectPhotoTable(){
    if(photoTable == nullptr){
        photoTable = new QSqlTableModel (this,m_db);
        photoTable->setTable("photo");
        photoTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
        photoTable->select();
        photoTable->setHeaderData(0,Qt::Horizontal, QObject::tr("File Name"));
        photoTable->setHeaderData(1,Qt::Horizontal, QObject::tr("Album Name"));
        photoTable->setHeaderData(2,Qt::Horizontal, QObject::tr("Status"));
        photoTable->setHeaderData(3,Qt::Horizontal, QObject::tr("Date Added"));
        photoTable->setHeaderData(4,Qt::Horizontal, QObject::tr("Date Modified"));
        photoTable->setHeaderData(5,Qt::Horizontal, QObject::tr("Path"));

        qDebug() << "Photo table is linked to sql table model";
    }
}
void DBmanager::addPhoto(QString const &filename,
                          QString const &album,
                          QString const &status,
                          QString const &dateAdded,
                          QString const &dateModified,
                          QString const &path){
    if(photoTable == nullptr){
        qDebug() << " Photo table pointer is NULL";
    }else{
        int row = photoTable->rowCount();
        QModelIndexList list =  photoTable->match(photoTable->index(0,photoTable->fieldIndex("filename")),Qt::DisplayRole,filename,-1);
        /* Prevent duplicate */
        if (list.count() == 0){
            photoTable->insertRows(row,1);
            photoTable->setData((photoTable->index(row,photoTable->fieldIndex("filename"))),filename);
            photoTable->setData((photoTable->index(row,photoTable->fieldIndex("album"))),album);
            photoTable->setData((photoTable->index(row,photoTable->fieldIndex("status"))),status);
            photoTable->setData((photoTable->index(row,photoTable->fieldIndex("dateAdded"))),dateAdded);
            photoTable->setData((photoTable->index(row,photoTable->fieldIndex("dateModified"))),dateModified);
            photoTable->setData((photoTable->index(row,photoTable->fieldIndex("path"))),path);
            photoTable->submitAll();
            showMessage("Photo added");
        }else
            showMessage("Error. Photo exists");

    }
}
void DBmanager::clearPhoto(){
    if(photoTable == nullptr){
        qDebug() << " Photo table pointer is NULL";
    }else{
        photoTable->removeRows(0,photoTable->rowCount());
        photoTable->submitAll();
    }
}

void DBmanager::removePhoto(int row){
    if(photoTable == nullptr){
        qDebug() << " Photo table pointer is NULL";
    }else{
        photoTable->removeRow(row);
        photoTable->submitAll();
    }
}
void DBmanager::setPhotoStatus(int row, QString const &status){
    if(photoTable == nullptr){
        qDebug() << " Photo table pointer is NULL";
    }else{
        photoTable->setData(photoTable->index(row,photoTable->fieldIndex("status")),status);
        photoTable->submitAll();
    }
}

void DBmanager::setAlbumName(int row, QString const &album){
    if(photoTable == nullptr){
        qDebug() << " Photo table pointer is NULL";
    }else{
        photoTable->setData(photoTable->index(row,photoTable->fieldIndex("album")),album);
        photoTable->submitAll();
    }
}

void DBmanager::setPhotoStatusByPath(QString const &path, QString const &status){
//    qDebug() << "Set status photo by path...";
//    QSqlTableModel model (this,m_db);
//    model.setTable("photo");
//    model.setEditStrategy(QSqlTableModel::OnManualSubmit);
//    /* remove the 1200px from the file path */
//    QString filter = QString("path = \"%1\"").arg(path);
//    filter.replace("/1200px","");
//    model.setFilter(filter);
//    model.select();
//    if(model.rowCount()>0){
//        model.setData(photoTable->index(0,model.fieldIndex("status")),status);
//        model.submitAll();
//    }
//    emit finished();

    /* remove the 1200px from the file path */
    QString searchKey = path;
    searchKey.replace("/1200px","");

    QModelIndexList list =  photoTable->match(photoTable->index(0,photoTable->fieldIndex("path")),Qt::DisplayRole,searchKey,-1);
    if(list.count() > 0){
        foreach(QModelIndex i, list){
           setPhotoStatus(i.row(),status);
        }
    }
    emit finished();

}
QSqlTableModel *DBmanager::getPhotoTable(){
    if(photoTable == nullptr)
        qDebug() << "Photo table pointer is NULL";
     return photoTable;
}


/*********************************** END **************************************/

/************************** Watched folder Table ******************************/
void DBmanager::createWatchedTable(){
    QSqlQuery query;
    if(m_db.isOpen()){
        if(m_db.tables().contains("watched")){
            qDebug() << "Watched table exists.";
            connectWatchedTable();
        }else{
            qDebug() << "create Watched table  in db";
            if(!query.exec("create table watched (folder varchar(255),"
                                                  "status varchar(20),"
                                                  "num_file int,"
                                                  "dateAdded varchar(255),"
                                                  "dateModified varchar(255),"
                                                  "path varchar(255),"
                                                  "primary key(folder,num_file,path))"))
                qDebug() << query.lastError().text();
             else
                connectWatchedTable();
        }
    }
}

void DBmanager::connectWatchedTable(){
    if(watchedTable == nullptr){
        watchedTable = new QSqlTableModel (this,m_db);
        watchedTable->setTable("watched");
        watchedTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
        watchedTable->select();
        watchedTable->setHeaderData(0,Qt::Horizontal, QObject::tr("Folder Name"));
        watchedTable->setHeaderData(2,Qt::Horizontal, QObject::tr("Status"));
        watchedTable->setHeaderData(1,Qt::Horizontal, QObject::tr("No. File"));
        watchedTable->setHeaderData(3,Qt::Horizontal, QObject::tr("Date Added"));
        watchedTable->setHeaderData(4,Qt::Horizontal, QObject::tr("Date Modified"));
        watchedTable->setHeaderData(5,Qt::Horizontal, QObject::tr("Path"));
        qDebug() << "Watched table is linked to sql database";
    }
}

void DBmanager::addWatched(QString const &folderName,
                            QString const &status,
                            int num_file,
                            QString const &dateAdded,
                            QString const &dateModified,
                            QString const &path){
    if(watchedTable == nullptr){
        qDebug() << " Watched table pointer is NULL";
    }else{
        /* Insert into table, Unique Key prevents duplicate */
        int row = watchedTable->rowCount();
        QModelIndexList list =  watchedTable->match(watchedTable->index(0,watchedTable->fieldIndex("folder")),Qt::DisplayRole,folderName,-1);
        /* Prevent duplicate */
       if(list.count() ==0){
            watchedTable->insertRows(row,1);
            watchedTable->setData((watchedTable->index(row,watchedTable->fieldIndex("folder"))),folderName);
            watchedTable->setData((watchedTable->index(row,watchedTable->fieldIndex("status"))),status);
            watchedTable->setData((watchedTable->index(row,watchedTable->fieldIndex("num_file"))),num_file);
            watchedTable->setData((watchedTable->index(row,watchedTable->fieldIndex("dateAdded"))),dateAdded);
            watchedTable->setData((watchedTable->index(row,watchedTable->fieldIndex("dateModified"))),dateModified);
            watchedTable->setData((watchedTable->index(row,watchedTable->fieldIndex("path"))),path);
            watchedTable->submitAll();
            showMessage("Watched Folder added");
        }else
           showMessage("Error. Watched Folder exists");
       }
}
void DBmanager::clearWatched(){
    if(watchedTable != nullptr){
        watchedTable->removeRows(0,watchedTable->rowCount());
        watchedTable->submitAll();
    }else{
        qDebug() << "Watched pointer is null";
    }
}
void DBmanager::removeWatched(int row){
    if(watchedTable != nullptr){
        watchedTable->removeRow(row);
        watchedTable->submitAll();
    }else{
        qDebug() << "Watched pointer is null";
    }
}

void DBmanager::setWatchedStatus(int row, QString const &status){
    if(watchedTable != nullptr){
        watchedTable->setData(watchedTable->index(row,watchedTable->fieldIndex("status")),status);
        watchedTable->submitAll();
    }else{
        qDebug() << "Watched pointer is null";
    }
}

void DBmanager::setWatchedNumFile(int row, int num_file){
    if(watchedTable != nullptr){
        watchedTable->setData(watchedTable->index(row,watchedTable->fieldIndex("num_file")),num_file);
        watchedTable->submitAll();
    }else{
        qDebug() << "Watched pointer is null";
    }
}

QSqlTableModel *DBmanager::getWatchedTable(){
    if(watchedTable == nullptr)
        qDebug() << "Watched pointer is null";
    return watchedTable;
}
/*********************************** END **************************************/

/***************************** SMS Table **************************************/

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
        smsTable->setHeaderData(0,Qt::Horizontal, QObject::tr("Phone Number"));
        smsTable->setHeaderData(1,Qt::Horizontal, QObject::tr("Carrier"));
        smsTable->setHeaderData(2,Qt::Horizontal, QObject::tr("Status"));
        smsTable->setHeaderData(3,Qt::Horizontal, QObject::tr("No. File"));
        smsTable->setHeaderData(4,Qt::Horizontal, QObject::tr("Date Added"));
        smsTable->setHeaderData(5,Qt::Horizontal, QObject::tr("Path"));

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
        /* Have to do a more complex search query here */
        if(!smsExists(phone,carrier,path)){
            int row = smsTable->rowCount();
            smsTable->insertRows(row,1);
            smsTable->setData((smsTable->index(row,smsTable->fieldIndex("phone"))),phone);
            smsTable->setData((smsTable->index(row,smsTable->fieldIndex("carrier"))),carrier);
            smsTable->setData((smsTable->index(row,smsTable->fieldIndex("status"))),status);
            smsTable->setData((smsTable->index(row,smsTable->fieldIndex("num_file"))),num_file);
            smsTable->setData((smsTable->index(row,smsTable->fieldIndex("dateAdded"))),dateAdded);
            smsTable->setData((smsTable->index(row,smsTable->fieldIndex("path"))),path);
            smsTable->submitAll();
            showMessage("SMS added");
        }
        else{
            showMessage("Error. SMS exists");
        }
    }
}

bool DBmanager::smsExists(QString const &phone,
                           QString const &carrier,
                           QString const &path){
    QModelIndexList list =  smsTable->match(smsTable->index(0,smsTable->fieldIndex("phone")),Qt::DisplayRole,phone,-1);
    foreach(QModelIndex i, list){
        if(smsTable->record(i.row()).value("carrier").toString() == carrier && smsTable->record(i.row()).value("path").toString() == path){
            return true;
        }
    }
    return false;

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
    }
    return smsTable;
}
/********************************* END **************************************/


/************************** Email Table **************************************/

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
        emailTable->setHeaderData(0,Qt::Horizontal, QObject::tr("Email Address"));
        emailTable->setHeaderData(1,Qt::Horizontal, QObject::tr("Status"));
        emailTable->setHeaderData(2,Qt::Horizontal, QObject::tr("No. File"));
        emailTable->setHeaderData(3,Qt::Horizontal, QObject::tr("Date Added"));
        emailTable->setHeaderData(4,Qt::Horizontal, QObject::tr("Path"));
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
        if(!emailExists(email,path)){
            int row = emailTable->rowCount();
            emailTable->insertRows(row,1);
            emailTable->setData((emailTable->index(row,emailTable->fieldIndex("email"))),email);
            emailTable->setData((emailTable->index(row,emailTable->fieldIndex("status"))),status);
            emailTable->setData((emailTable->index(row,emailTable->fieldIndex("num_file"))),num_file);
            emailTable->setData((emailTable->index(row,emailTable->fieldIndex("dateAdded"))),dateAdded);
            emailTable->setData((emailTable->index(row,emailTable->fieldIndex("path"))),path);
            emailTable->submitAll();
            showMessage("Email added");

        }else
            showMessage("Error. Email exists");
    }
}

bool DBmanager::emailExists(QString const &email,
                           QString const &path){
    QModelIndexList list =  emailTable->match(emailTable->index(0,emailTable->fieldIndex("email")),Qt::DisplayRole,email,-1);
    foreach(QModelIndex i, list){
        if(emailTable->record(i.row()).value("path").toString() == path){
            return true;
        }
    }
    return false;

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
    if(emailTable == nullptr)
        qDebug() << " Email table pointer is NULL";
    return emailTable;

}
/************************** END **************************************/
