#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class TestCase
{
public:
    enum Status {
        NotRun,
        Running,
        Passed,
        Failed
    };
    
    TestCase() : status(NotRun), responseTime(0) {}
    
    int id;
    QString name;
    QString method;
    QString url;
    QString headers;
    QString body;
    QString expectedStatusCode;
    QString expectedResponse;
    Status status;
    QString actualResponse;
    QString actualStatusCode;
    qint64 responseTime;
    QString errorMessage;
};

class TestRunner : public QObject
{
    Q_OBJECT
    
public:
    explicit TestRunner(QObject *parent = nullptr);
    
    void addTest(const TestCase &testCase);
    void runAllTests();
    void runTest(int testId);
    void clearTests();
    
    QList<TestCase> getTests() const { return m_testCases; }
    TestCase getTest(int testId) const;
    
signals:
    void testStarted(int testId);
    void testCompleted(int testId, bool passed);
    void allTestsCompleted(int totalTests, int passedTests, int failedTests);
    
private slots:
    void onTestReplyFinished();
    
private:
    void runNextTest();
    bool evaluateTestResult(const TestCase &testCase, const QString &response, int statusCode);
    
    QList<TestCase> m_testCases;
    QNetworkAccessManager *m_networkManager;
    int m_currentTestIndex;
    bool m_isRunning;
    QNetworkReply *m_currentReply;
    qint64 m_testStartTime;
};

#endif // TESTRUNNER_H