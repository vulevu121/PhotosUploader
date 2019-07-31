#include "gmail.h"

GMAIL::GMAIL(QObject *parent) : QObject(parent)
{
    auth.SetScope("GMAIL"); // default scope is google photo
    auth.Authenticate();
    connect(&auth,SIGNAL(tokenReady(QString)),this,SLOT(SetAccessToken(QString)));

}

void GMAIL::SetToEmail(QString email){
    receiverEmail = email;
}
void GMAIL::SetFromEmail(QString email){
    senderEmail = email;
    }
void GMAIL::SetAccessToken(QString token){
    qDebug() << "Gmail Access Token is set";
    accessToken = token;
    emit authenticated();
}

void GMAIL::SetAlbumURL(QString url){
    albumURL = url;
    emit linkReady();
}

void GMAIL::SetBody(QString body){
    emailBody = body;
}

void GMAIL::SetSubject(QString subject){
    emailSubject = subject;
}

void GMAIL::SendEmail(){
    qDebug() << "Sending email with link...";

    if (manager == nullptr) {
         manager = new QNetworkAccessManager(this);
     }

    if(senderEmail.isEmpty()){
        qDebug() << "FROM email was not provided. Please set.";
        return;
    }else if(receiverEmail.isEmpty()){
        qDebug() << "TO email not provided. Please set.";
        return;
    }else if(emailSubject.isEmpty()){
        qDebug() << "Subject is not provided. Please set.";
        return;
    }else if(emailBody.isEmpty()){
        qDebug() << "Body is not provided. Please set.";
        return;
    }

    /* NEED A LINE BETWEEN SUBJECT AND BODY FOR THIS TO WORK */
    QString message ("From:"+ senderEmail+ "\n"
                    "To:" + receiverEmail+ "\n"
                     "Subject: " + emailSubject +"\n"
                     "\n" + emailBody + "\n"
                     "Shareable Album Link:" + albumURL );

    QByteArray encoded = message.toUtf8().toBase64(QByteArray::Base64UrlEncoding);
//    qDebug() << encoded ;


    /* Ensure encoded message is URL safe */
    encoded.replace("+","-");
    encoded.replace("/","_");
//    encoded.replace("=","*");

//    qDebug() << encoded ;

    QJsonObject jsonObj;
    jsonObj ["raw"] = QString(encoded);


    QJsonDocument doc (jsonObj);

    QByteArray jsonRequest = doc.toJson(QJsonDocument::Compact);
    QByteArray postDataSize = QByteArray::number(jsonRequest.size());

    QString endPoint ("https://www.googleapis.com/gmail/v1/users/");
    QUrl sendURL(endPoint + "me"+ "/messages/send");
    QNetworkRequest sendReq(sendURL);
//    qDebug() << sendURL;

    sendReq.setRawHeader("Authorization","Bearer "+ accessToken.toUtf8());
    sendReq.setRawHeader("Content-Type","application/json");
    sendReq.setRawHeader("Content-Length", postDataSize);

    manager->post(sendReq,jsonRequest);

    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(SendEmailReply(QNetworkReply*)));
}

void GMAIL::SendEmailReply(QNetworkReply * reply){
    if(reply->error()) {
        qDebug() << "Sending Email Error" << reply->readAll();
        manager->disconnect();

    } else {
        qDebug() << "Sending Email Success";

        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject jsonObj = jsonDoc.object();

//        qDebug() << jsonObj;

        manager->disconnect();

     }

}
