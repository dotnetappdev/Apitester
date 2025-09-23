#ifndef RESPONSEPANEL_H
#define RESPONSEPANEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTreeWidget>

class ResponsePanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit ResponsePanel(QWidget *parent = nullptr);
    
public slots:
    void displayResponse(const QString &response, int statusCode, qint64 responseTime, const QString &headers);
    void clearResponse();
    
private:
    void setupUI();
    void formatJsonResponse(const QString &response);
    void updateStatusInfo(int statusCode, qint64 responseTime, const QString &response);
    void addJsonObjectToTree(QTreeWidgetItem *parent, const QJsonObject &obj);
    void addJsonArrayToTree(QTreeWidgetItem *parent, const QJsonArray &array);
    QString getStatusText(int statusCode);
    QString getStatusColorClass(int statusCode);
    
    // UI Components
    QLabel *m_statusLabel;
    QLabel *m_timeLabel;
    QLabel *m_sizeLabel;
    QTabWidget *m_responseTabs;
    QTextEdit *m_bodyTextEdit;
    QTextEdit *m_headersTextEdit;
    QTextEdit *m_rawTextEdit;
    QTreeWidget *m_jsonTreeWidget;
};

#endif // RESPONSEPANEL_H