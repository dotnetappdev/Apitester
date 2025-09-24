#include "TestExplorer.h"
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidgetItem>
#include <QtCore/QDateTime>
#include <QtGui/QIcon>

TestExplorer::TestExplorer(QWidget *parent)
    : QWidget(parent)
    , m_testRunner(new TestRunner(this))
    , m_updateTimer(new QTimer(this))
    , m_totalTests(0)
    , m_completedTests(0)
    , m_passedTests(0)
    , m_failedTests(0)
{
    setupUI();
    
    // Connect test runner signals
    connect(m_testRunner, &TestRunner::testStarted, this, &TestExplorer::onTestStarted);
    connect(m_testRunner, &TestRunner::testCompleted, this, &TestExplorer::onTestCompleted);
    connect(m_testRunner, &TestRunner::allTestsCompleted, this, &TestExplorer::onAllTestsCompleted);
    
    // Connect tree selection
    connect(m_testTree, &QTreeWidget::itemSelectionChanged, this, &TestExplorer::onTestItemSelectionChanged);
    
    // Update timer for progress indication
    m_updateTimer->setSingleShot(false);
    m_updateTimer->setInterval(100);
    connect(m_updateTimer, &QTimer::timeout, this, &TestExplorer::updateSummary);
}

void TestExplorer::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // Button layout
    m_buttonLayout = new QHBoxLayout();
    
    m_runAllButton = new QPushButton("▶ Run All Tests");
    m_runAllButton->setStyleSheet("QPushButton { font-weight: bold; color: #28a745; }");
    connect(m_runAllButton, &QPushButton::clicked, this, &TestExplorer::runAllTests);
    m_buttonLayout->addWidget(m_runAllButton);
    
    m_runSelectedButton = new QPushButton("▶ Run Selected");
    connect(m_runSelectedButton, &QPushButton::clicked, this, &TestExplorer::runSelectedTest);
    m_buttonLayout->addWidget(m_runSelectedButton);
    
    m_refreshButton = new QPushButton("🔄 Refresh");
    connect(m_refreshButton, &QPushButton::clicked, this, &TestExplorer::refreshTests);
    m_buttonLayout->addWidget(m_refreshButton);
    
    m_buttonLayout->addStretch();
    
    m_mainLayout->addLayout(m_buttonLayout);
    
    // Summary and progress
    m_summaryLabel = new QLabel("Ready - No tests loaded");
    m_summaryLabel->setStyleSheet("font-weight: bold; margin: 5px 0px;");
    m_mainLayout->addWidget(m_summaryLabel);
    
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_mainLayout->addWidget(m_progressBar);
    
    // Splitter with test tree and details
    m_splitter = new QSplitter(Qt::Vertical);
    
    // Test tree
    m_testTree = new QTreeWidget();
    m_testTree->setHeaderLabels({"Test Name", "Status", "Method", "URL", "Time (ms)"});
    m_testTree->setAlternatingRowColors(true);
    m_testTree->header()->setStretchLastSection(false);
    m_testTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_testTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_testTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_testTree->header()->setSectionResizeMode(3, QHeaderView::Interactive);
    m_testTree->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    
    m_splitter->addWidget(m_testTree);
    
    // Details text
    m_detailsText = new QTextEdit();
    m_detailsText->setReadOnly(true);
    m_detailsText->setMaximumHeight(200);
    m_detailsText->setPlainText("Select a test to see details...");
    
    m_splitter->addWidget(m_detailsText);
    m_splitter->setSizes({300, 200});
    
    m_mainLayout->addWidget(m_splitter);
}

void TestExplorer::addTestFromRequest(int requestId, const QString &name, const QString &method,
                                    const QString &url, const QString &headers, const QString &body)
{
    TestCase testCase;
    testCase.id = requestId;
    testCase.name = name;
    testCase.method = method;
    testCase.url = url;
    testCase.headers = headers;
    testCase.body = body;
    testCase.expectedStatusCode = "200"; // Default expectation
    testCase.status = TestCase::NotRun;
    
    m_testRunner->addTest(testCase);
    
    // Add to tree
    QTreeWidgetItem *item = new QTreeWidgetItem(m_testTree);
    item->setText(0, name);
    item->setText(1, getStatusText(TestCase::NotRun));
    item->setText(2, method);
    item->setText(3, url.length() > 50 ? url.left(47) + "..." : url);
    item->setText(4, "-");
    item->setData(0, Qt::UserRole, requestId);
    
    // Set status icon
    item->setIcon(1, QIcon()); // Will be updated with actual icons
    
    updateSummary();
}

void TestExplorer::clearTests()
{
    m_testRunner->clearTests();
    m_testTree->clear();
    m_detailsText->setPlainText("Select a test to see details...");
    
    m_totalTests = 0;
    m_completedTests = 0;
    m_passedTests = 0;
    m_failedTests = 0;
    
    updateSummary();
}

void TestExplorer::runAllTests()
{
    if (m_testRunner->getTests().isEmpty()) {
        return;
    }
    
    m_completedTests = 0;
    m_passedTests = 0;
    m_failedTests = 0;
    m_totalTests = m_testRunner->getTests().size();
    
    m_progressBar->setMaximum(m_totalTests);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(true);
    
    m_runAllButton->setEnabled(false);
    m_runSelectedButton->setEnabled(false);
    
    m_updateTimer->start();
    m_testRunner->runAllTests();
}

void TestExplorer::runSelectedTest()
{
    QTreeWidgetItem *selectedItem = m_testTree->currentItem();
    if (!selectedItem) {
        return;
    }
    
    int testId = selectedItem->data(0, Qt::UserRole).toInt();
    
    m_runAllButton->setEnabled(false);
    m_runSelectedButton->setEnabled(false);
    
    m_testRunner->runTest(testId);
}

void TestExplorer::refreshTests()
{
    // This would typically reload tests from the database
    // For now, just update the display
    updateSummary();
}

void TestExplorer::onTestStarted(int testId)
{
    updateTestItem(testId, TestCase::Running);
}

void TestExplorer::onTestCompleted(int testId, bool passed)
{
    TestCase::Status status = passed ? TestCase::Passed : TestCase::Failed;
    updateTestItem(testId, status);
    
    m_completedTests++;
    if (passed) {
        m_passedTests++;
    } else {
        m_failedTests++;
    }
    
    m_progressBar->setValue(m_completedTests);
}

void TestExplorer::onAllTestsCompleted(int totalTests, int passedTests, int failedTests)
{
    m_updateTimer->stop();
    m_progressBar->setVisible(false);
    
    m_runAllButton->setEnabled(true);
    m_runSelectedButton->setEnabled(true);
    
    m_totalTests = totalTests;
    m_passedTests = passedTests;
    m_failedTests = failedTests;
    m_completedTests = totalTests;
    
    updateSummary();
}

void TestExplorer::onTestItemSelectionChanged()
{
    QTreeWidgetItem *selectedItem = m_testTree->currentItem();
    if (!selectedItem) {
        m_detailsText->setPlainText("Select a test to see details...");
        return;
    }
    
    int testId = selectedItem->data(0, Qt::UserRole).toInt();
    TestCase testCase = m_testRunner->getTest(testId);
    
    showTestDetails(testCase);
}

void TestExplorer::updateTestItem(int testId, TestCase::Status status)
{
    for (int i = 0; i < m_testTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_testTree->topLevelItem(i);
        if (item->data(0, Qt::UserRole).toInt() == testId) {
            item->setText(1, getStatusText(status));
            
            // Update response time if available
            TestCase testCase = m_testRunner->getTest(testId);
            if (testCase.responseTime > 0) {
                item->setText(4, QString::number(testCase.responseTime));
            }
            
            // Color coding
            if (status == TestCase::Passed) {
                item->setBackground(1, QColor(40, 167, 69, 50)); // Green tint
            } else if (status == TestCase::Failed) {
                item->setBackground(1, QColor(220, 53, 69, 50)); // Red tint
            } else if (status == TestCase::Running) {
                item->setBackground(1, QColor(255, 193, 7, 50)); // Yellow tint
            } else {
                item->setBackground(1, QColor()); // Clear background
            }
            
            break;
        }
    }
}

void TestExplorer::updateSummary()
{
    if (m_totalTests == 0) {
        m_summaryLabel->setText("Ready - No tests loaded");
    } else if (m_completedTests < m_totalTests) {
        m_summaryLabel->setText(QString("Running tests... %1/%2 completed")
                               .arg(m_completedTests).arg(m_totalTests));
    } else {
        QString result = QString("Completed: %1 tests, %2 passed, %3 failed")
                        .arg(m_totalTests).arg(m_passedTests).arg(m_failedTests);
        
        if (m_failedTests == 0 && m_totalTests > 0) {
            m_summaryLabel->setStyleSheet("font-weight: bold; color: #28a745; margin: 5px 0px;");
        } else if (m_failedTests > 0) {
            m_summaryLabel->setStyleSheet("font-weight: bold; color: #dc3545; margin: 5px 0px;");
        } else {
            m_summaryLabel->setStyleSheet("font-weight: bold; margin: 5px 0px;");
        }
        
        m_summaryLabel->setText(result);
    }
}

QString TestExplorer::getStatusIcon(TestCase::Status status)
{
    switch (status) {
        case TestCase::Passed: return "✓";
        case TestCase::Failed: return "✗";
        case TestCase::Running: return "⟳";
        default: return "○";
    }
}

QString TestExplorer::getStatusText(TestCase::Status status)
{
    switch (status) {
        case TestCase::Passed: return getStatusIcon(status) + " Passed";
        case TestCase::Failed: return getStatusIcon(status) + " Failed";
        case TestCase::Running: return getStatusIcon(status) + " Running";
        default: return getStatusIcon(status) + " Not Run";
    }
}

void TestExplorer::showTestDetails(const TestCase &testCase)
{
    QString details;
    details += QString("Test Name: %1\n").arg(testCase.name);
    details += QString("Method: %1\n").arg(testCase.method);
    details += QString("URL: %1\n").arg(testCase.url);
    details += QString("Status: %1\n").arg(getStatusText(testCase.status));
    
    if (testCase.responseTime > 0) {
        details += QString("Response Time: %1 ms\n").arg(testCase.responseTime);
    }
    
    if (!testCase.actualStatusCode.isEmpty()) {
        details += QString("Status Code: %1\n").arg(testCase.actualStatusCode);
    }
    
    if (!testCase.headers.isEmpty()) {
        details += QString("\nRequest Headers:\n%1\n").arg(testCase.headers);
    }
    
    if (!testCase.body.isEmpty()) {
        details += QString("\nRequest Body:\n%1\n").arg(testCase.body);
    }
    
    if (!testCase.actualResponse.isEmpty()) {
        details += QString("\nResponse:\n%1\n").arg(testCase.actualResponse.left(500));
        if (testCase.actualResponse.length() > 500) {
            details += "... (truncated)\n";
        }
    }
    
    if (!testCase.errorMessage.isEmpty()) {
        details += QString("\nError: %1\n").arg(testCase.errorMessage);
    }
    
    m_detailsText->setPlainText(details);
}