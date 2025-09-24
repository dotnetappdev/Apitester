#include "HttpClient.h"
#include "DatabaseManager.h"
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QElapsedTimer>
#include <QtNetwork/QNetworkRequest>

HttpClient::HttpClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_startTime(0)
{
}

void HttpClient::sendRequest(const QString &method, const QString &url, 
                           const QString &headers, const QString &body)
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    
    QNetworkRequest request = createRequest(url, headers);
    QByteArray requestBody = body.toUtf8();
    
    m_startTime = QDateTime::currentMSecsSinceEpoch();
    
    QString methodUpper = method.toUpper();
    
    if (methodUpper == "GET") {
        m_currentReply = m_networkManager->get(request);
    } else if (methodUpper == "POST") {
        m_currentReply = m_networkManager->post(request, requestBody);
    } else if (methodUpper == "PUT") {
        m_currentReply = m_networkManager->put(request, requestBody);
    } else if (methodUpper == "DELETE") {
        m_currentReply = m_networkManager->deleteResource(request);
    } else if (methodUpper == "HEAD") {
        m_currentReply = m_networkManager->head(request);
    } else if (methodUpper == "PATCH") {
        m_currentReply = m_networkManager->sendCustomRequest(request, "PATCH", requestBody);
    } else if (methodUpper == "OPTIONS") {
        m_currentReply = m_networkManager->sendCustomRequest(request, "OPTIONS");
    } else {
        // Default to GET for unknown methods
        m_currentReply = m_networkManager->get(request);
    }
    
    connect(m_currentReply, &QNetworkReply::finished, this, &HttpClient::onReplyFinished);
}

QNetworkRequest HttpClient::createRequest(const QString &url, const QString &headers)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    
    // Set default headers
    request.setHeader(QNetworkRequest::UserAgentHeader, "ApiTester/1.0");
    request.setRawHeader("Accept", "*/*");
    
    // Parse and set custom headers
    if (!headers.isEmpty()) {
        QStringList headerLines = headers.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : headerLines) {
            int colonIndex = line.indexOf(':');
            if (colonIndex > 0) {
                QString headerName = line.left(colonIndex).trimmed();
                QString headerValue = line.mid(colonIndex + 1).trimmed();
                request.setRawHeader(headerName.toUtf8(), headerValue.toUtf8());
            }
        }
    }
    
    return request;
}

void HttpClient::onReplyFinished()
{
    if (!m_currentReply) {
        return;
    }
    
    qint64 responseTime = QDateTime::currentMSecsSinceEpoch() - m_startTime;
    int statusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    // Get response body
    QByteArray responseData = m_currentReply->readAll();
    QString response = QString::fromUtf8(responseData);
    
    // Get response headers
    QStringList responseHeaders;
    const QList<QNetworkReply::RawHeaderPair> headers = m_currentReply->rawHeaderPairs();
    for (const auto &header : headers) {
        responseHeaders << QString("%1: %2").arg(QString::fromUtf8(header.first), 
                                                QString::fromUtf8(header.second));
    }
    QString headerString = responseHeaders.join('\n');
    
    // Handle network errors
    if (m_currentReply->error() != QNetworkReply::NoError && statusCode == 0) {
        response = QString("Network Error: %1").arg(m_currentReply->errorString());
        statusCode = -1;
    }
    
    // Save to history
    DatabaseManager::instance().saveToHistory(
        "GET", // We'll need to pass the actual method later
        m_currentReply->url().toString(),
        "", // We'll need to pass the actual request headers later
        "", // We'll need to pass the actual request body later
        response,
        statusCode,
        responseTime
    );
    
    emit responseReceived(response, statusCode, responseTime, headerString);
    emit requestFinished();
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}