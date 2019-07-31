#ifndef GOOGLEOAUTH2_H
#define GOOGLEOAUTH2_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QUrl>
#include <QWebEngineView>
#include <QDeadlineTimer>
#include <QTimer>

class GoogleOAuth2 : public QObject
{
    Q_OBJECT
public:
    explicit GoogleOAuth2(QObject *parent = nullptr);

private:
    QNetworkAccessManager *manager = nullptr;
    QWebEngineView *view = nullptr;

    QJsonObject settingsObject;
    QString jsonFilePath = QString("C:/Users/khuon/Documents/GooglePhoto/client_secret_1044474243779-a1gndnc2as4cc5c6ufksmbetoafi5mcr.apps.googleusercontent.com.json");
    QString scope = QString("?scope=https://www.googleapis.com/auth/photoslibrary.sharing");
    QString response_type;
    QString redirect_uri;
    QString client_id;
    QString client_secret;
    QString grant_type = QString("&grant_type=authorization_code");

    QString authEndpoint;
    QString tokenEndpoint;
    QString authCode;
    QString accessToken;
    QString refreshToken;
    int expireTime;  // Use for countdown. Unit is millisecond
    int cautionOffset = 10000; // refresh access token 10 second before it expires

private slots:
    void ExchangeAccessToken();
    void ExchangeTokenReply(QNetworkReply *reply);
    void AuthenticateReply(QNetworkReply *reply);
    void AuthenticateRedirectReply(QUrl url);
    void RefreshAccessToken();
    void RefreshAccessTokenReply(QNetworkReply *reply);
public slots:
    void Authenticate();
    void SetScope(QString RequestScope = "PHOTO");
    void SetRawScope(QString RawScope);  // use to set a scope different from the default options

signals:
    void tokenReady(QString token);
    void scopeSet();
    void authCodeReady();
};

#endif // GOOGLEOAUTH2_H
