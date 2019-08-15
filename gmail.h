#ifndef GMAIL_H
#define GMAIL_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QUrl>
#include "googleoauth2.h"

class GMAIL : public QObject
{
    Q_OBJECT
public:
    explicit GMAIL(QObject *parent = nullptr);
    ~GMAIL();
private:
    QNetworkAccessManager *manager = nullptr;
    GoogleOAuth2 * auth  = nullptr;
    QString albumURL = QString("No URL available");
    QString accessToken;
    QString receiverEmail = QString("khuong.dinh.ng@gmail.com");
    QString senderEmail = QString("khuongnguyensac@gmail.com");
    QString emailSubject = QString("Subject is not available");
    QString emailBody = QString("Body is not available");

signals:
    void authenticated();
    void linkReady();

public slots:
    void SetAlbumURL(QString const &url);
    void SendEmail();
    void SetToEmail(QString const &email);
    void SetFromEmail(QString const &email);
    void SetSubject(QString const &sub);
    void SetBody(QString const &body);


private slots:
    void SendEmailReply(QNetworkReply * reply);
    void SetAccessToken(QString const &token);


};

#endif // GMAIL_H
