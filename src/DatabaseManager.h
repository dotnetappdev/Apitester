#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

class DatabaseManager : public QObject
{
    Q_OBJECT
    
public:
    static DatabaseManager& instance();
    
    bool initialize();
    bool initialize(const QString &customPath);
    QSqlDatabase database() const { return m_database; }
    QString getCurrentDatabasePath() const { return m_currentDatabasePath; }
    
    // Collection management
    bool createCollection(const QString &name, const QString &description = QString());
    bool deleteCollection(int collectionId);
    bool updateCollection(int collectionId, const QString &name, const QString &description);
    
    // Request management
    bool saveRequest(int collectionId, const QString &name, const QString &method, 
                    const QString &url, const QString &headers, const QString &body,
                    const QString &parameters);
    bool deleteRequest(int requestId);
    bool updateRequest(int requestId, const QString &name, const QString &method,
                      const QString &url, const QString &headers, const QString &body,
                      const QString &parameters);
    
    // History management
    bool saveToHistory(const QString &method, const QString &url, const QString &headers,
                      const QString &body, const QString &response, int statusCode,
                      qint64 responseTime);
    bool clearHistory();
    
private:
    explicit DatabaseManager(QObject *parent = nullptr);
    bool createTables();
    
    QSqlDatabase m_database;
    QString m_currentDatabasePath;
};

#endif // DATABASEMANAGER_H