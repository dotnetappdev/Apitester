#include "TestRunner.h"
#include <QtCore/QUrl>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtCore/QDateTime>
#include <QtNetwork/QNetworkRequest>

TestRunner::TestRunner(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentTestIndex(-1)
    , m_isRunning(false)
    , m_currentReply(nullptr)
    , m_testStartTime(0)
{
}

void TestRunner::addTest(const TestCase &testCase)
{
    m_testCases.append(testCase);
}

void TestRunner::runAllTests()
{
    if (m_isRunning) {
        return;
    }
    
    // Reset all tests to NotRun status
    for (int i = 0; i < m_testCases.size(); ++i) {
        m_testCases[i].status = TestCase::NotRun;
        m_testCases[i].actualResponse.clear();
        m_testCases[i].actualStatusCode.clear();
        m_testCases[i].errorMessage.clear();
        m_testCases[i].responseTime = 0;
    }
    
    m_currentTestIndex = 0;
    m_isRunning = true;
    
    if (m_testCases.isEmpty()) {
        m_isRunning = false;
        emit allTestsCompleted(0, 0, 0);
        return;
    }
    
    runNextTest();
}

void TestRunner::runTest(int testId)
{
    if (m_isRunning) {
        return;
    }
    
    // Find test by id
    int index = -1;
    for (int i = 0; i < m_testCases.size(); ++i) {
        if (m_testCases[i].id == testId) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        return;
    }
    
    m_currentTestIndex = index;
    m_isRunning = true;
    
    runNextTest();
}

void TestRunner::clearTests()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    
    m_testCases.clear();
    m_currentTestIndex = -1;
    m_isRunning = false;
}

TestCase TestRunner::getTest(int testId) const
{
    for (const TestCase &testCase : m_testCases) {
        if (testCase.id == testId) {
            return testCase;
        }
    }
    return TestCase();
}

void TestRunner::runNextTest()
{
    if (m_currentTestIndex < 0 || m_currentTestIndex >= m_testCases.size()) {
        // All tests completed
        m_isRunning = false;
        
        int passedCount = 0;
        int failedCount = 0;
        for (const TestCase &testCase : m_testCases) {
            if (testCase.status == TestCase::Passed) {
                passedCount++;
            } else if (testCase.status == TestCase::Failed) {
                failedCount++;
            }
        }
        
        emit allTestsCompleted(m_testCases.size(), passedCount, failedCount);
        return;
    }
    
    TestCase &currentTest = m_testCases[m_currentTestIndex];
    currentTest.status = TestCase::Running;
    
    emit testStarted(currentTest.id);
    
    // Create network request
    QNetworkRequest request;
    request.setUrl(QUrl(currentTest.url));
    request.setHeader(QNetworkRequest::UserAgentHeader, "ApiTester/1.0");
    
    // Set custom headers
    if (!currentTest.headers.isEmpty()) {
        QStringList headerLines = currentTest.headers.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : headerLines) {
            int colonIndex = line.indexOf(':');
            if (colonIndex > 0) {
                QString headerName = line.left(colonIndex).trimmed();
                QString headerValue = line.mid(colonIndex + 1).trimmed();
                request.setRawHeader(headerName.toUtf8(), headerValue.toUtf8());
            }
        }
    }
    
    // Send request based on method
    QString method = currentTest.method.toUpper();
    QByteArray requestBody = currentTest.body.toUtf8();
    
    m_testStartTime = QDateTime::currentMSecsSinceEpoch();
    
    if (method == "GET") {
        m_currentReply = m_networkManager->get(request);
    } else if (method == "POST") {
        m_currentReply = m_networkManager->post(request, requestBody);
    } else if (method == "PUT") {
        m_currentReply = m_networkManager->put(request, requestBody);
    } else if (method == "DELETE") {
        m_currentReply = m_networkManager->deleteResource(request);
    } else if (method == "PATCH") {
        m_currentReply = m_networkManager->sendCustomRequest(request, "PATCH", requestBody);
    } else if (method == "HEAD") {
        m_currentReply = m_networkManager->head(request);
    } else if (method == "OPTIONS") {
        m_currentReply = m_networkManager->sendCustomRequest(request, "OPTIONS");
    } else {
        // Default to GET for unknown methods
        m_currentReply = m_networkManager->get(request);
    }
    
    connect(m_currentReply, &QNetworkReply::finished, this, &TestRunner::onTestReplyFinished);
}

void TestRunner::onTestReplyFinished()
{
    if (!m_currentReply || m_currentTestIndex < 0 || m_currentTestIndex >= m_testCases.size()) {
        return;
    }
    
    TestCase &currentTest = m_testCases[m_currentTestIndex];
    
    qint64 responseTime = QDateTime::currentMSecsSinceEpoch() - m_testStartTime;
    int statusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString response = QString::fromUtf8(m_currentReply->readAll());
    
    currentTest.responseTime = responseTime;
    currentTest.actualStatusCode = QString::number(statusCode);
    currentTest.actualResponse = response;
    
    // Handle network errors
    if (m_currentReply->error() != QNetworkReply::NoError && statusCode == 0) {
        currentTest.errorMessage = m_currentReply->errorString();
        currentTest.status = TestCase::Failed;
    } else {
        // Evaluate test result
        bool passed = evaluateTestResult(currentTest, response, statusCode);
        currentTest.status = passed ? TestCase::Passed : TestCase::Failed;
    }
    
    emit testCompleted(currentTest.id, currentTest.status == TestCase::Passed);
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
    
    // Move to next test or finish
    if (m_currentTestIndex == -1) {
        // Single test run
        m_isRunning = false;
        return;
    }
    
    m_currentTestIndex++;
    runNextTest();
}

bool TestRunner::evaluateTestResult(const TestCase &testCase, const QString &response, int statusCode)
{
    // Check status code if specified
    if (!testCase.expectedStatusCode.isEmpty()) {
        bool ok;
        int expectedStatus = testCase.expectedStatusCode.toInt(&ok);
        if (ok && statusCode != expectedStatus) {
            return false;
        }
    }
    
    // Check response content if specified
    if (!testCase.expectedResponse.isEmpty()) {
        // Simple string contains check for now
        if (!response.contains(testCase.expectedResponse, Qt::CaseInsensitive)) {
            return false;
        }
    }
    
    // If we get here and no specific checks failed, consider it passed
    // (or if no expectations were set, just getting a response is considered success)
    return true;
}