#ifndef REQUESTPANEL_H
#define REQUESTPANEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>

class HttpClient;
class JsonSyntaxHighlighter;
class XmlSyntaxHighlighter;

class RequestPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit RequestPanel(QWidget *parent = nullptr);
    
    QString getMethod() const;
    QString getUrl() const;
    QString getHeaders() const;
    QString getBody() const;
    QString getParameters() const;
    
    void setMethod(const QString &method);
    void setUrl(const QString &url);
    void setHeaders(const QString &headers);
    void setBody(const QString &body);
    void setParameters(const QString &parameters);
    
signals:
    void responseReceived(const QString &response, int statusCode, qint64 responseTime, const QString &headers);
    void requestStarted();
    void requestFinished();
    
private slots:
    void sendRequest();
    void onResponseReceived(const QString &response, int statusCode, qint64 responseTime, const QString &headers);
    void addHeader();
    void removeHeader();
    void addParameter();
    void removeParameter();
    void onBodyTypeChanged();
    void onBodyTextChanged();
    
private:
    void setupUI();
    void setupMethodCombo();
    void applySyntaxHighlighting();
    void updateBodyEditor();
    QString getHttpMethodIcon(const QString &method);
    QString getHttpMethodColor(const QString &method);
    void populateHeadersFromTable();
    void populateParametersFromTable();
    void populateTableFromHeaders();
    void populateTableFromParameters();
    
    // UI Components
    QComboBox *m_methodCombo;
    QLineEdit *m_urlEdit;
    QPushButton *m_sendButton;
    QTabWidget *m_requestTabs;
    
    // Headers tab
    QTableWidget *m_headersTable;
    QPushButton *m_addHeaderButton;
    QPushButton *m_removeHeaderButton;
    
    // Body tab
    QComboBox *m_bodyTypeCombo;
    QTextEdit *m_bodyEdit;
    
    // Parameters tab
    QTableWidget *m_parametersTable;
    QPushButton *m_addParameterButton;
    QPushButton *m_removeParameterButton;
    
    // Auth tab
    QComboBox *m_authTypeCombo;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_tokenEdit;
    
    HttpClient *m_httpClient;
    
    // Syntax highlighters
    JsonSyntaxHighlighter *m_jsonHighlighter;
    XmlSyntaxHighlighter *m_xmlHighlighter;
};

#endif // REQUESTPANEL_H