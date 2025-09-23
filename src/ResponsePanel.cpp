#include "ResponsePanel.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtGui/QFont>
#include <QtWidgets/QTreeWidgetItem>

ResponsePanel::ResponsePanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ResponsePanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Status bar
    QHBoxLayout *statusLayout = new QHBoxLayout();
    
    m_statusLabel = new QLabel("Status: Ready");
    m_statusLabel->setStyleSheet("font-weight: bold;");
    statusLayout->addWidget(m_statusLabel);
    
    m_timeLabel = new QLabel("Time: -");
    statusLayout->addWidget(m_timeLabel);
    
    m_sizeLabel = new QLabel("Size: -");
    statusLayout->addWidget(m_sizeLabel);
    
    statusLayout->addStretch();
    mainLayout->addLayout(statusLayout);
    
    // Response tabs
    m_responseTabs = new QTabWidget();
    
    // Body tab - with sub-tabs for different views
    QTabWidget *bodyTabs = new QTabWidget();
    
    // Pretty formatted body
    m_bodyTextEdit = new QTextEdit();
    m_bodyTextEdit->setReadOnly(true);
    QFont font("Consolas, Monaco, monospace");
    font.setPointSize(10);
    m_bodyTextEdit->setFont(font);
    bodyTabs->addTab(m_bodyTextEdit, "Pretty");
    
    // JSON tree view
    m_jsonTreeWidget = new QTreeWidget();
    m_jsonTreeWidget->setHeaderLabel("JSON Structure");
    bodyTabs->addTab(m_jsonTreeWidget, "JSON");
    
    // Raw body
    m_rawTextEdit = new QTextEdit();
    m_rawTextEdit->setReadOnly(true);
    m_rawTextEdit->setFont(font);
    bodyTabs->addTab(m_rawTextEdit, "Raw");
    
    m_responseTabs->addTab(bodyTabs, "Body");
    
    // Headers tab
    m_headersTextEdit = new QTextEdit();
    m_headersTextEdit->setReadOnly(true);
    m_headersTextEdit->setFont(font);
    m_responseTabs->addTab(m_headersTextEdit, "Headers");
    
    mainLayout->addWidget(m_responseTabs);
}

void ResponsePanel::displayResponse(const QString &response, int statusCode, 
                                  qint64 responseTime, const QString &headers)
{
    // Update status information
    updateStatusInfo(statusCode, responseTime, response);
    
    // Display headers
    m_headersTextEdit->setPlainText(headers);
    
    // Display raw response
    m_rawTextEdit->setPlainText(response);
    
    // Try to format as JSON if possible
    formatJsonResponse(response);
    
    // Set the response in the pretty view
    m_bodyTextEdit->setPlainText(response);
    
    // Try to pretty print JSON
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError) {
        QString prettyJson = jsonDoc.toJson(QJsonDocument::Indented);
        m_bodyTextEdit->setPlainText(prettyJson);
    }
}

void ResponsePanel::clearResponse()
{
    m_bodyTextEdit->clear();
    m_headersTextEdit->clear();
    m_rawTextEdit->clear();
    m_jsonTreeWidget->clear();
    m_statusLabel->setText("Status: Ready");
    m_timeLabel->setText("Time: -");
    m_sizeLabel->setText("Size: -");
}

void ResponsePanel::formatJsonResponse(const QString &response)
{
    m_jsonTreeWidget->clear();
    
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        QTreeWidgetItem *errorItem = new QTreeWidgetItem(m_jsonTreeWidget);
        errorItem->setText(0, "Invalid JSON: " + error.errorString());
        return;
    }
    
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(m_jsonTreeWidget);
    
    if (jsonDoc.isObject()) {
        rootItem->setText(0, "Object");
        addJsonObjectToTree(rootItem, jsonDoc.object());
    } else if (jsonDoc.isArray()) {
        rootItem->setText(0, "Array");
        addJsonArrayToTree(rootItem, jsonDoc.array());
    } else {
        rootItem->setText(0, "Value: " + jsonDoc.toJson(QJsonDocument::Compact));
    }
    
    m_jsonTreeWidget->expandToDepth(2);
}

void ResponsePanel::addJsonObjectToTree(QTreeWidgetItem *parent, const QJsonObject &obj)
{
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parent);
        item->setText(0, it.key());
        
        const QJsonValue &value = it.value();
        if (value.isObject()) {
            item->setText(0, item->text(0) + " (Object)");
            addJsonObjectToTree(item, value.toObject());
        } else if (value.isArray()) {
            item->setText(0, item->text(0) + " (Array)");
            addJsonArrayToTree(item, value.toArray());
        } else {
            QString valueText;
            switch (value.type()) {
                case QJsonValue::String:
                    valueText = "\"" + value.toString() + "\"";
                    break;
                case QJsonValue::Double:
                    valueText = QString::number(value.toDouble());
                    break;
                case QJsonValue::Bool:
                    valueText = value.toBool() ? "true" : "false";
                    break;
                case QJsonValue::Null:
                    valueText = "null";
                    break;
                default:
                    valueText = value.toVariant().toString();
            }
            item->setText(0, item->text(0) + ": " + valueText);
        }
    }
}

void ResponsePanel::addJsonArrayToTree(QTreeWidgetItem *parent, const QJsonArray &array)
{
    for (int i = 0; i < array.size(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parent);
        item->setText(0, QString("[%1]").arg(i));
        
        const QJsonValue &value = array[i];
        if (value.isObject()) {
            item->setText(0, item->text(0) + " (Object)");
            addJsonObjectToTree(item, value.toObject());
        } else if (value.isArray()) {
            item->setText(0, item->text(0) + " (Array)");
            addJsonArrayToTree(item, value.toArray());
        } else {
            QString valueText;
            switch (value.type()) {
                case QJsonValue::String:
                    valueText = "\"" + value.toString() + "\"";
                    break;
                case QJsonValue::Double:
                    valueText = QString::number(value.toDouble());
                    break;
                case QJsonValue::Bool:
                    valueText = value.toBool() ? "true" : "false";
                    break;
                case QJsonValue::Null:
                    valueText = "null";
                    break;
                default:
                    valueText = value.toVariant().toString();
            }
            item->setText(0, item->text(0) + ": " + valueText);
        }
    }
}

void ResponsePanel::updateStatusInfo(int statusCode, qint64 responseTime, const QString &response)
{
    // Update status
    QString statusText = getStatusText(statusCode);
    QString statusColor = getStatusColorClass(statusCode);
    m_statusLabel->setText(QString("Status: %1 %2").arg(statusCode).arg(statusText));
    m_statusLabel->setStyleSheet(QString("font-weight: bold; color: %1;").arg(statusColor));
    
    // Update time
    m_timeLabel->setText(QString("Time: %1 ms").arg(responseTime));
    
    // Update size
    int sizeBytes = response.toUtf8().size();
    QString sizeText;
    if (sizeBytes < 1024) {
        sizeText = QString("%1 B").arg(sizeBytes);
    } else if (sizeBytes < 1024 * 1024) {
        sizeText = QString("%.1f KB").arg(sizeBytes / 1024.0);
    } else {
        sizeText = QString("%.1f MB").arg(sizeBytes / (1024.0 * 1024.0));
    }
    m_sizeLabel->setText("Size: " + sizeText);
}

QString ResponsePanel::getStatusText(int statusCode)
{
    switch (statusCode) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case -1: return "Network Error";
        default: return "";
    }
}

QString ResponsePanel::getStatusColorClass(int statusCode)
{
    if (statusCode >= 200 && statusCode < 300) {
        return "#22c55e"; // Green
    } else if (statusCode >= 300 && statusCode < 400) {
        return "#f59e0b"; // Yellow
    } else if (statusCode >= 400 && statusCode < 500) {
        return "#ef4444"; // Red
    } else if (statusCode >= 500) {
        return "#dc2626"; // Dark red
    } else {
        return "#6b7280"; // Gray
    }
}