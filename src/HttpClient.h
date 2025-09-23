#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class HttpClient : public QObject
{
    Q_OBJECT
    
public:
    explicit HttpClient(QObject *parent = nullptr);
    
    void sendRequest(const QString &method, const QString &url, 
                    const QString &headers, const QString &body);
    
signals:
    void responseReceived(const QString &response, int statusCode, qint64 responseTime, const QString &headers);
    void requestFinished();
    
private slots:
    void onReplyFinished();
    
private:
    QNetworkRequest createRequest(const QString &url, const QString &headers);
    QByteArray getMethodVerb(const QString &method);
    
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QTimer m_timer;
    qint64 m_startTime;
};

#endif // HTTPCLIENT_H