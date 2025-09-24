#include "RequestPanel.h"
#include "HttpClient.h"
#include "SyntaxHighlighter.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QCheckBox>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtGui/QFont>

RequestPanel::RequestPanel(QWidget *parent)
    : QWidget(parent)
    , m_httpClient(new HttpClient(this))
    , m_jsonHighlighter(nullptr)
    , m_xmlHighlighter(nullptr)
{
    setupUI();
    
    // Initialize body editor state
    updateBodyEditor();
    
    connect(m_httpClient, &HttpClient::responseReceived, 
            this, &RequestPanel::onResponseReceived);
    connect(m_httpClient, &HttpClient::requestFinished,
            this, &RequestPanel::requestFinished);
}

void RequestPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Request line with enhanced Postman-like styling
    QHBoxLayout *requestLayout = new QHBoxLayout();
    requestLayout->setSpacing(8);
    
    // Method combo with icons and colors
    m_methodCombo = new QComboBox();
    setupMethodCombo();
    requestLayout->addWidget(m_methodCombo);
    
    // URL input with enhanced styling
    m_urlEdit = new QLineEdit();
    m_urlEdit->setPlaceholderText("Enter request URL (e.g., https://api.example.com/endpoint)");
    m_urlEdit->setStyleSheet(R"(
        QLineEdit {
            padding: 8px 12px;
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            font-size: 14px;
            background-color: white;
        }
        QLineEdit:focus {
            border-color: #007acc;
            outline: none;
        }
        QLineEdit:hover {
            border-color: #c0c0c0;
        }
    )");
    requestLayout->addWidget(m_urlEdit);
    
    // Enhanced Send button with Postman-like styling
    m_sendButton = new QPushButton("Send");
    m_sendButton->setFixedSize(100, 36);
    m_sendButton->setStyleSheet(R"(
        QPushButton {
            background-color: #ff6c37;
            color: white;
            border: none;
            border-radius: 6px;
            font-size: 14px;
            font-weight: bold;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #e55a2b;
        }
        QPushButton:pressed {
            background-color: #d44a20;
        }
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
    )");
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
    
    // Body tab with enhanced styling and syntax highlighting
    QWidget *bodyWidget = new QWidget();
    QVBoxLayout *bodyLayout = new QVBoxLayout(bodyWidget);
    bodyLayout->setSpacing(8);
    
    // Body type selection with enhanced styling
    QHBoxLayout *bodyTypeLayout = new QHBoxLayout();
    QLabel *bodyTypeLabel = new QLabel("Body Type:");
    bodyTypeLabel->setStyleSheet("font-weight: bold; color: #333333;");
    bodyTypeLayout->addWidget(bodyTypeLabel);
    
    m_bodyTypeCombo = new QComboBox();
    m_bodyTypeCombo->addItems({"none", "raw", "form-data", "x-www-form-urlencoded", "binary"});
    m_bodyTypeCombo->setStyleSheet(R"(
        QComboBox {
            padding: 6px 12px;
            border: 1px solid #d0d0d0;
            border-radius: 4px;
            background-color: white;
            min-width: 150px;
        }
        QComboBox:hover {
            border-color: #007acc;
        }
        QComboBox::drop-down {
            border: none;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 4px solid #666666;
        }
    )");
    connect(m_bodyTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &RequestPanel::onBodyTypeChanged);
    bodyTypeLayout->addWidget(m_bodyTypeCombo);
    bodyTypeLayout->addStretch();
    bodyLayout->addLayout(bodyTypeLayout);
    
    // Enhanced body editor with syntax highlighting
    m_bodyEdit = new QTextEdit();
    m_bodyEdit->setPlaceholderText("Enter request body...\n\nFor JSON, the editor will automatically apply syntax highlighting.");
    
    // Set font to monospace for better code readability
    QFont monoFont("Consolas, Monaco, 'Courier New', monospace");
    monoFont.setPointSize(11);
    m_bodyEdit->setFont(monoFont);
    
    m_bodyEdit->setStyleSheet(R"(
        QTextEdit {
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            padding: 10px;
            background-color: #fafafa;
            line-height: 1.4;
        }
        QTextEdit:focus {
            border-color: #007acc;
            background-color: white;
        }
    )");
    
    // Connect text change signal for syntax highlighting
    connect(m_bodyEdit, &QTextEdit::textChanged, this, &RequestPanel::onBodyTextChanged);
    
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
    return m_methodCombo->currentData().toString();
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
    // Find the index by data value instead of display text
    for (int i = 0; i < m_methodCombo->count(); ++i) {
        if (m_methodCombo->itemData(i).toString() == method) {
            m_methodCombo->setCurrentIndex(i);
            return;
        }
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

void RequestPanel::setupMethodCombo()
{
    // Clear existing items
    m_methodCombo->clear();
    
    // Add HTTP methods with icons and colors
    QStringList methods = {"GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS"};
    
    for (const QString &method : methods) {
        QString icon = getHttpMethodIcon(method);
        QString displayText = QString("%1 %2").arg(icon, method);
        m_methodCombo->addItem(displayText, method);
    }
    
    // Enhanced styling for method combo box
    m_methodCombo->setStyleSheet(R"(
        QComboBox {
            padding: 8px 12px;
            border: 2px solid #e0e0e0;
            border-radius: 6px;
            background-color: white;
            font-size: 14px;
            font-weight: bold;
            min-width: 120px;
        }
        QComboBox:hover {
            border-color: #c0c0c0;
        }
        QComboBox:focus {
            border-color: #007acc;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #666666;
        }
        QComboBox QAbstractItemView {
            border: 1px solid #d0d0d0;
            border-radius: 4px;
            background-color: white;
            selection-background-color: #e3f2fd;
        }
    )");
    
    // Set default to GET
    m_methodCombo->setCurrentIndex(0);
}

QString RequestPanel::getHttpMethodIcon(const QString &method)
{
    if (method == "GET") return "🔵";
    else if (method == "POST") return "🟢";
    else if (method == "PUT") return "🟡";
    else if (method == "DELETE") return "🔴";
    else if (method == "PATCH") return "🟠";
    else if (method == "HEAD") return "⚪";
    else if (method == "OPTIONS") return "🟣";
    return "⚫";
}

QString RequestPanel::getHttpMethodColor(const QString &method)
{
    if (method == "GET") return "#007acc";
    else if (method == "POST") return "#28a745";
    else if (method == "PUT") return "#ffc107";
    else if (method == "DELETE") return "#dc3545";
    else if (method == "PATCH") return "#fd7e14";
    else if (method == "HEAD") return "#6c757d";
    else if (method == "OPTIONS") return "#6f42c1";
    return "#343a40";
}

void RequestPanel::applySyntaxHighlighting()
{
    QString bodyType = m_bodyTypeCombo->currentText();
    QString bodyText = m_bodyEdit->toPlainText().trimmed();
    
    // Remove existing highlighters
    if (m_jsonHighlighter) {
        delete m_jsonHighlighter;
        m_jsonHighlighter = nullptr;
    }
    if (m_xmlHighlighter) {
        delete m_xmlHighlighter;
        m_xmlHighlighter = nullptr;
    }
    
    // Apply appropriate highlighting based on content type or body content
    if (bodyType == "raw" && !bodyText.isEmpty()) {
        // Auto-detect JSON
        if (bodyText.startsWith('{') || bodyText.startsWith('[')) {
            m_jsonHighlighter = new JsonSyntaxHighlighter(m_bodyEdit->document());
        }
        // Auto-detect XML
        else if (bodyText.startsWith('<')) {
            m_xmlHighlighter = new XmlSyntaxHighlighter(m_bodyEdit->document());
        }
    }
}

void RequestPanel::updateBodyEditor()
{
    QString bodyType = m_bodyTypeCombo->currentText();
    
    if (bodyType == "none") {
        m_bodyEdit->setEnabled(false);
        m_bodyEdit->setPlaceholderText("Select a body type to enable the editor");
    } else {
        m_bodyEdit->setEnabled(true);
        
        if (bodyType == "raw") {
            m_bodyEdit->setPlaceholderText("Enter raw body content...\n\nSupported formats:\n• JSON (auto-highlighted)\n• XML (auto-highlighted)\n• Plain text");
        } else if (bodyType == "form-data") {
            m_bodyEdit->setPlaceholderText("Enter form data (key=value pairs, one per line):\n\nExample:\nname=John Doe\nemail=john@example.com");
        } else if (bodyType == "x-www-form-urlencoded") {
            m_bodyEdit->setPlaceholderText("Enter URL-encoded data:\n\nExample:\nname=John+Doe&email=john%40example.com");
        } else {
            m_bodyEdit->setPlaceholderText("Enter request body content...");
        }
    }
    
    applySyntaxHighlighting();
}

void RequestPanel::onBodyTypeChanged()
{
    updateBodyEditor();
}

void RequestPanel::onBodyTextChanged()
{
    // Re-apply syntax highlighting when text changes
    applySyntaxHighlighting();
}