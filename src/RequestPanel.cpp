#include "RequestPanel.h"
#include "HttpClient.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QCheckBox>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

RequestPanel::RequestPanel(QWidget *parent)
    : QWidget(parent)
    , m_httpClient(new HttpClient(this))
{
    setupUI();
    
    connect(m_httpClient, &HttpClient::responseReceived, 
            this, &RequestPanel::onResponseReceived);
    connect(m_httpClient, &HttpClient::requestFinished,
            this, &RequestPanel::requestFinished);
}

void RequestPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Request line
    QHBoxLayout *requestLayout = new QHBoxLayout();
    
    m_methodCombo = new QComboBox();
    m_methodCombo->addItems({"GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS"});
    m_methodCombo->setFixedWidth(100);
    requestLayout->addWidget(m_methodCombo);
    
    m_urlEdit = new QLineEdit();
    m_urlEdit->setPlaceholderText("Enter request URL...");
    requestLayout->addWidget(m_urlEdit);
    
    m_sendButton = new QPushButton("Send");
    m_sendButton->setFixedWidth(80);
    connect(m_sendButton, &QPushButton::clicked, this, &RequestPanel::sendRequest);
    requestLayout->addWidget(m_sendButton);
    
    mainLayout->addLayout(requestLayout);
    
    // Request details tabs
    m_requestTabs = new QTabWidget();
    
    // Headers tab
    QWidget *headersWidget = new QWidget();
    QVBoxLayout *headersLayout = new QVBoxLayout(headersWidget);
    
    QHBoxLayout *headersButtonLayout = new QHBoxLayout();
    m_addHeaderButton = new QPushButton("Add Header");
    m_removeHeaderButton = new QPushButton("Remove Header");
    connect(m_addHeaderButton, &QPushButton::clicked, this, &RequestPanel::addHeader);
    connect(m_removeHeaderButton, &QPushButton::clicked, this, &RequestPanel::removeHeader);
    headersButtonLayout->addWidget(m_addHeaderButton);
    headersButtonLayout->addWidget(m_removeHeaderButton);
    headersButtonLayout->addStretch();
    headersLayout->addLayout(headersButtonLayout);
    
    m_headersTable = new QTableWidget(0, 3);
    m_headersTable->setHorizontalHeaderLabels({"Enabled", "Key", "Value"});
    m_headersTable->horizontalHeader()->setStretchLastSection(true);
    m_headersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_headersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    headersLayout->addWidget(m_headersTable);
    
    m_requestTabs->addTab(headersWidget, "Headers");
    
    // Body tab
    QWidget *bodyWidget = new QWidget();
    QVBoxLayout *bodyLayout = new QVBoxLayout(bodyWidget);
    
    QHBoxLayout *bodyTypeLayout = new QHBoxLayout();
    bodyTypeLayout->addWidget(new QLabel("Body Type:"));
    m_bodyTypeCombo = new QComboBox();
    m_bodyTypeCombo->addItems({"none", "raw", "form-data", "x-www-form-urlencoded", "binary"});
    bodyTypeLayout->addWidget(m_bodyTypeCombo);
    bodyTypeLayout->addStretch();
    bodyLayout->addLayout(bodyTypeLayout);
    
    m_bodyEdit = new QTextEdit();
    m_bodyEdit->setPlaceholderText("Enter request body...");
    bodyLayout->addWidget(m_bodyEdit);
    
    m_requestTabs->addTab(bodyWidget, "Body");
    
    // Parameters tab
    QWidget *parametersWidget = new QWidget();
    QVBoxLayout *parametersLayout = new QVBoxLayout(parametersWidget);
    
    QHBoxLayout *parametersButtonLayout = new QHBoxLayout();
    m_addParameterButton = new QPushButton("Add Parameter");
    m_removeParameterButton = new QPushButton("Remove Parameter");
    connect(m_addParameterButton, &QPushButton::clicked, this, &RequestPanel::addParameter);
    connect(m_removeParameterButton, &QPushButton::clicked, this, &RequestPanel::removeParameter);
    parametersButtonLayout->addWidget(m_addParameterButton);
    parametersButtonLayout->addWidget(m_removeParameterButton);
    parametersButtonLayout->addStretch();
    parametersLayout->addLayout(parametersButtonLayout);
    
    m_parametersTable = new QTableWidget(0, 3);
    m_parametersTable->setHorizontalHeaderLabels({"Enabled", "Key", "Value"});
    m_parametersTable->horizontalHeader()->setStretchLastSection(true);
    m_parametersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_parametersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    parametersLayout->addWidget(m_parametersTable);
    
    m_requestTabs->addTab(parametersWidget, "Parameters");
    
    // Auth tab
    QWidget *authWidget = new QWidget();
    QFormLayout *authLayout = new QFormLayout(authWidget);
    
    m_authTypeCombo = new QComboBox();
    m_authTypeCombo->addItems({"No Auth", "Basic Auth", "Bearer Token", "API Key"});
    authLayout->addRow("Type:", m_authTypeCombo);
    
    m_usernameEdit = new QLineEdit();
    authLayout->addRow("Username:", m_usernameEdit);
    
    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    authLayout->addRow("Password:", m_passwordEdit);
    
    m_tokenEdit = new QLineEdit();
    authLayout->addRow("Token:", m_tokenEdit);
    
    m_requestTabs->addTab(authWidget, "Auth");
    
    mainLayout->addWidget(m_requestTabs);
    
    // Add initial header row
    addHeader();
    addParameter();
}

void RequestPanel::sendRequest()
{
    QString method = m_methodCombo->currentText();
    QString url = m_urlEdit->text().trimmed();
    
    if (url.isEmpty()) {
        return;
    }
    
    // Add parameters to URL if any
    populateParametersFromTable();
    QString parameters = getParameters();
    if (!parameters.isEmpty() && method == "GET") {
        QUrl urlObj(url);
        QUrlQuery query(urlObj.query());
        
        QStringList paramPairs = parameters.split('&', Qt::SkipEmptyParts);
        for (const QString &pair : paramPairs) {
            QStringList keyValue = pair.split('=');
            if (keyValue.size() == 2) {
                query.addQueryItem(keyValue[0], keyValue[1]);
            }
        }
        
        urlObj.setQuery(query);
        url = urlObj.toString();
    }
    
    populateHeadersFromTable();
    QString headers = getHeaders();
    
    // Add authentication headers
    QString authType = m_authTypeCombo->currentText();
    if (authType == "Basic Auth" && !m_usernameEdit->text().isEmpty()) {
        QString credentials = QString("%1:%2").arg(m_usernameEdit->text(), m_passwordEdit->text());
        QByteArray encoded = credentials.toUtf8().toBase64();
        if (!headers.isEmpty()) headers += "\n";
        headers += QString("Authorization: Basic %1").arg(QString::fromUtf8(encoded));
    } else if (authType == "Bearer Token" && !m_tokenEdit->text().isEmpty()) {
        if (!headers.isEmpty()) headers += "\n";
        headers += QString("Authorization: Bearer %1").arg(m_tokenEdit->text());
    } else if (authType == "API Key" && !m_tokenEdit->text().isEmpty()) {
        if (!headers.isEmpty()) headers += "\n";
        headers += QString("X-API-Key: %1").arg(m_tokenEdit->text());
    }
    
    QString body;
    if (method != "GET" && method != "HEAD") {
        body = m_bodyEdit->toPlainText();
        
        // Set content-type based on body type
        QString bodyType = m_bodyTypeCombo->currentText();
        if (bodyType == "raw" && !body.isEmpty()) {
            // Try to detect JSON
            if (body.trimmed().startsWith('{') || body.trimmed().startsWith('[')) {
                if (!headers.isEmpty()) headers += "\n";
                headers += "Content-Type: application/json";
            } else {
                if (!headers.isEmpty()) headers += "\n";
                headers += "Content-Type: text/plain";
            }
        } else if (bodyType == "x-www-form-urlencoded") {
            if (!headers.isEmpty()) headers += "\n";
            headers += "Content-Type: application/x-www-form-urlencoded";
        }
    }
    
    emit requestStarted();
    m_httpClient->sendRequest(method, url, headers, body);
}

void RequestPanel::onResponseReceived(const QString &response, int statusCode, 
                                    qint64 responseTime, const QString &headers)
{
    emit responseReceived(response, statusCode, responseTime, headers);
}

void RequestPanel::addHeader()
{
    int row = m_headersTable->rowCount();
    m_headersTable->insertRow(row);
    
    QCheckBox *enabledCheck = new QCheckBox();
    enabledCheck->setChecked(true);
    m_headersTable->setCellWidget(row, 0, enabledCheck);
    
    m_headersTable->setItem(row, 1, new QTableWidgetItem(""));
    m_headersTable->setItem(row, 2, new QTableWidgetItem(""));
}

void RequestPanel::removeHeader()
{
    int currentRow = m_headersTable->currentRow();
    if (currentRow >= 0) {
        m_headersTable->removeRow(currentRow);
    }
}

void RequestPanel::addParameter()
{
    int row = m_parametersTable->rowCount();
    m_parametersTable->insertRow(row);
    
    QCheckBox *enabledCheck = new QCheckBox();
    enabledCheck->setChecked(true);
    m_parametersTable->setCellWidget(row, 0, enabledCheck);
    
    m_parametersTable->setItem(row, 1, new QTableWidgetItem(""));
    m_parametersTable->setItem(row, 2, new QTableWidgetItem(""));
}

void RequestPanel::removeParameter()
{
    int currentRow = m_parametersTable->currentRow();
    if (currentRow >= 0) {
        m_parametersTable->removeRow(currentRow);
    }
}

QString RequestPanel::getMethod() const
{
    return m_methodCombo->currentText();
}

QString RequestPanel::getUrl() const
{
    return m_urlEdit->text();
}

QString RequestPanel::getHeaders() const
{
    QStringList headersList;
    for (int row = 0; row < m_headersTable->rowCount(); ++row) {
        QCheckBox *enabledCheck = qobject_cast<QCheckBox*>(m_headersTable->cellWidget(row, 0));
        if (enabledCheck && enabledCheck->isChecked()) {
            QTableWidgetItem *keyItem = m_headersTable->item(row, 1);
            QTableWidgetItem *valueItem = m_headersTable->item(row, 2);
            if (keyItem && valueItem && !keyItem->text().trimmed().isEmpty()) {
                headersList << QString("%1: %2").arg(keyItem->text().trimmed(), valueItem->text());
            }
        }
    }
    return headersList.join('\n');
}

QString RequestPanel::getBody() const
{
    return m_bodyEdit->toPlainText();
}

QString RequestPanel::getParameters() const
{
    QStringList paramsList;
    for (int row = 0; row < m_parametersTable->rowCount(); ++row) {
        QCheckBox *enabledCheck = qobject_cast<QCheckBox*>(m_parametersTable->cellWidget(row, 0));
        if (enabledCheck && enabledCheck->isChecked()) {
            QTableWidgetItem *keyItem = m_parametersTable->item(row, 1);
            QTableWidgetItem *valueItem = m_parametersTable->item(row, 2);
            if (keyItem && valueItem && !keyItem->text().trimmed().isEmpty()) {
                paramsList << QString("%1=%2").arg(keyItem->text().trimmed(), valueItem->text());
            }
        }
    }
    return paramsList.join('&');
}

void RequestPanel::setMethod(const QString &method)
{
    int index = m_methodCombo->findText(method);
    if (index >= 0) {
        m_methodCombo->setCurrentIndex(index);
    }
}

void RequestPanel::setUrl(const QString &url)
{
    m_urlEdit->setText(url);
}

void RequestPanel::setHeaders(const QString &headers)
{
    // Clear existing headers
    m_headersTable->setRowCount(0);
    
    // Parse headers and populate table
    QStringList headerLines = headers.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : headerLines) {
        int colonIndex = line.indexOf(':');
        if (colonIndex > 0) {
            QString key = line.left(colonIndex).trimmed();
            QString value = line.mid(colonIndex + 1).trimmed();
            
            int row = m_headersTable->rowCount();
            m_headersTable->insertRow(row);
            
            QCheckBox *enabledCheck = new QCheckBox();
            enabledCheck->setChecked(true);
            m_headersTable->setCellWidget(row, 0, enabledCheck);
            
            m_headersTable->setItem(row, 1, new QTableWidgetItem(key));
            m_headersTable->setItem(row, 2, new QTableWidgetItem(value));
        }
    }
    
    // Add empty row for new headers
    if (headerLines.isEmpty()) {
        addHeader();
    }
}

void RequestPanel::setBody(const QString &body)
{
    m_bodyEdit->setPlainText(body);
}

void RequestPanel::setParameters(const QString &parameters)
{
    // Clear existing parameters
    m_parametersTable->setRowCount(0);
    
    // Parse parameters and populate table
    QStringList paramPairs = parameters.split('&', Qt::SkipEmptyParts);
    for (const QString &pair : paramPairs) {
        QStringList keyValue = pair.split('=');
        if (keyValue.size() >= 1) {
            QString key = keyValue[0];
            QString value = keyValue.size() > 1 ? keyValue[1] : "";
            
            int row = m_parametersTable->rowCount();
            m_parametersTable->insertRow(row);
            
            QCheckBox *enabledCheck = new QCheckBox();
            enabledCheck->setChecked(true);
            m_parametersTable->setCellWidget(row, 0, enabledCheck);
            
            m_parametersTable->setItem(row, 1, new QTableWidgetItem(key));
            m_parametersTable->setItem(row, 2, new QTableWidgetItem(value));
        }
    }
    
    // Add empty row for new parameters
    if (paramPairs.isEmpty()) {
        addParameter();
    }
}

void RequestPanel::populateHeadersFromTable()
{
    // This method is called before sending request to update internal headers string
    // The actual work is done in getHeaders()
}

void RequestPanel::populateParametersFromTable()
{
    // This method is called before sending request to update internal parameters string
    // The actual work is done in getParameters()  
}

void RequestPanel::populateTableFromHeaders()
{
    // This would be used when loading from database - implementation handled in setHeaders()
}

void RequestPanel::populateTableFromParameters()
{
    // This would be used when loading from database - implementation handled in setParameters()
}