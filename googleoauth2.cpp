#include "googleoauth2.h"

GoogleOAuth2::GoogleOAuth2(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(authCodeReady()),this,SLOT(ExchangeAccessToken()));
}
void GoogleOAuth2::SetScope(QString RequestScope){

    if(RequestScope == "GMAIL"){
        qDebug() << "Scope for Gmail";
        scope = QString("?scope=https://www.googleapis.com/auth/gmail.send"); // Create, read, update, and delete drafts. Send messages and drafts.
    }else{
        qDebug() << "Scope for Google Photo";
        scope = QString("?scope=https://www.googleapis.com/auth/photoslibrary.sharing"); // scope for sharing
    }
    emit scopeSet();
}

void GoogleOAuth2::SetRawScope(QString RawScope){
    scope = QString("?scope="+RawScope);

}


void GoogleOAuth2::Authenticate(){
    if (manager == nullptr) {
         manager = new QNetworkAccessManager(this);
     }

    QFile jsonFile(jsonFilePath);
    jsonFile.open(QFile::ReadOnly);
    QJsonDocument document = QJsonDocument().fromJson(jsonFile.readAll());

    const auto object = document.object();
    settingsObject = object["web"].toObject();
    authEndpoint = settingsObject["auth_uri"].toString();
    tokenEndpoint = settingsObject["token_uri"].toString() + "?";

    response_type = QString("&response_type=code");

    redirect_uri = QString("&redirect_uri=" + settingsObject["redirect_uris"].toArray()[0].toString());

    client_id = "&client_id=" + settingsObject["client_id"].toString();

    client_secret = "&client_secret=" + settingsObject["client_secret"].toString();

    QUrl url(authEndpoint + scope + response_type + redirect_uri + client_id);

//        qDebug() << url;
        QNetworkRequest req(url);

        manager->get(req);

        connect(this->manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(AuthenticateReply(QNetworkReply*)));
}

void GoogleOAuth2::AuthenticateReply(QNetworkReply *reply) {
    if(reply->error()) {
        qDebug() << reply->errorString();
    } else {
        qDebug() << "Access Code request success!";
        QUrl url(reply->url());
        view = new QWebEngineView();
        view->load(url);
//        view->show();
        connect(view,SIGNAL(urlChanged(QUrl)),this,SLOT(AuthenticateRedirectReply(QUrl)));
    }
    manager->disconnect();

}

void GoogleOAuth2::AuthenticateRedirectReply(QUrl url) {
    qDebug() << "Access Code Received!";
    QString url_string(url.toString());
//    qDebug() << url_string;


    url_string.replace("?","&");
    QStringList list  = url_string.split(QString("&"));

//    qDebug() << list;

    if (list[0] == settingsObject["redirect_uris"].toArray()[0].toString()){
        authCode = list.at(1);
//        qDebug() << authCode;
        emit authCodeReady();
    }
}

void GoogleOAuth2::ExchangeAccessToken(){
    qDebug() << "Exchanging Access Token...";

    /* Exchange the access code for access token */
    if (manager == nullptr) {
         manager = new QNetworkAccessManager(this);
     }


    QUrl urlToken(tokenEndpoint+ authCode+client_id+client_secret+redirect_uri+grant_type);
    QNetworkRequest req(urlToken);
    req.setRawHeader("Content-Type","application/x-www-form-urlencoded");

//    qDebug() << urlToken;
    QByteArray data;
    manager->post(req,data);

    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(ExchangeTokenReply(QNetworkReply*)));
}

void GoogleOAuth2::ExchangeTokenReply(QNetworkReply *reply) {
    if(reply->error()) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();
        qDebug() << jsonObject["error"].toObject()["message"].toString();
        manager->disconnect();

    } else {
        qDebug() << "Token Received!";
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();
        /* Extract the tokens */
        accessToken = jsonObject["access_token"].toString();
        expireTime  = jsonObject["expires_in"].toInt();

        /* Exchange new access token after expire time.
        APPARENTLY, THE RESPONSE DOES NOT CONTAIN REFRESH TOKEN
        Just have to Authenticate() after the expire time*/

        qDebug() << "Expire in" << expireTime;
        QTimer::singleShot(expireTime*1000 - cautionOffset,this,SLOT(Authenticate())); // Conversion to milisecond
        /* Use for testing only */
//        QTimer::singleShot(expireTime*2,this,SLOT(Authenticate())); // Conversion to milisecond

        qDebug() << "New Access Token:" << accessToken;
        /* disconnext previous connect */
        manager->disconnect();
        emit tokenReady(accessToken);

    }
}
/* Note that there are limits on the number of refresh tokens that will be issued;
 * one limit per client/user combination, and another per user across all clients*/

void GoogleOAuth2::RefreshAccessToken(){
    qDebug() << "Refreshing Access Token...";

    /* Exchange the access code for access token */
    if (manager == nullptr) {
         manager = new QNetworkAccessManager(this);
     }

    grant_type = QString("&grant_type=refresh_token");
    /* Prepend the keyword to build the API request*/
    refreshToken = QString("refresh_token="+refreshToken);

    QUrl urlToken(tokenEndpoint+refreshToken+client_id+client_secret+grant_type);
    QNetworkRequest req(urlToken);
    req.setRawHeader("Content-Type","application/x-www-form-urlencoded");

    qDebug() << urlToken;
    QByteArray data;
    manager->post(req,data);

    connect(this->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(RefreshAccessTokenReply(QNetworkReply*)));
}
void GoogleOAuth2::RefreshAccessTokenReply(QNetworkReply* reply){
    if(reply->error()) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();
        qDebug() << jsonObject["error"].toObject()["message"].toString();
        manager->disconnect();

    } else {
        qDebug() << "Access Token is renewed!";
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonDoc.object();
        /* Extract the tokens */
        accessToken = jsonObject["access_token"].toString();
        expireTime  = jsonObject["expires_in"].toInt();
        qDebug() <<  "Refreshed Access Token:" << accessToken;
        /* disconnext previous connect */
        manager->disconnect();
        /* Use the same signal so that the class uses googleoauth2 automatically
         * update their stored token */
        emit tokenReady(accessToken);

    }
}

