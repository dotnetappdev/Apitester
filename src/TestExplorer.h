#ifndef TESTEXPLORER_H
#define TESTEXPLORER_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtCore/QTimer>
#include "TestRunner.h"

class TestExplorer : public QWidget
{
    Q_OBJECT
    
public:
    explicit TestExplorer(QWidget *parent = nullptr);
    
    void addTestFromRequest(int requestId, const QString &name, const QString &method,
                           const QString &url, const QString &headers, const QString &body);
    void clearTests();
    
private slots:
    void runAllTests();
    void runSelectedTest();
    void refreshTests();
    void onTestStarted(int testId);
    void onTestCompleted(int testId, bool passed);
    void onAllTestsCompleted(int totalTests, int passedTests, int failedTests);
    void onTestItemSelectionChanged();
    
private:
    void setupUI();
    void updateTestItem(int testId, TestCase::Status status);
    void updateSummary();
    QString getStatusIcon(TestCase::Status status);
    QString getStatusText(TestCase::Status status);
    void showTestDetails(const TestCase &testCase);
    
    // UI Components
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_runAllButton;
    QPushButton *m_runSelectedButton;
    QPushButton *m_refreshButton;
    
    QLabel *m_summaryLabel;
    QProgressBar *m_progressBar;
    
    QSplitter *m_splitter;
    QTreeWidget *m_testTree;
    QTextEdit *m_detailsText;
    
    // Test functionality
    TestRunner *m_testRunner;
    QTimer *m_updateTimer;
    
    // Statistics
    int m_totalTests;
    int m_completedTests;
    int m_passedTests;
    int m_failedTests;
};

#endif // TESTEXPLORER_H