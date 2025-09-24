#include "DatabaseManager.h"
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

bool DatabaseManager::initialize()
{
    QSettings settings;
    QString customPath = settings.value("database/path", "").toString();
    
    if (!customPath.isEmpty()) {
        return initialize(customPath);
    }
    
    // Use default path
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return initialize(dataPath);
}

bool DatabaseManager::initialize(const QString &customPath)
{
    // Create database directory if it doesn't exist
    QDir().mkpath(customPath);
    
    m_currentDatabasePath = customPath + "/apitester.db";
    
    // Initialize SQLite database
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_currentDatabasePath);
    
    if (!m_database.open()) {
        qWarning() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }
    
    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);
    
    // Create collections table
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS collections (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            description TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )")) {
        qWarning() << "Failed to create collections table:" << query.lastError().text();
        return false;
    }
    
    // Create requests table
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS requests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            collection_id INTEGER,
            name TEXT NOT NULL,
            method TEXT NOT NULL,
            url TEXT NOT NULL,
            headers TEXT,
            body TEXT,
            parameters TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (collection_id) REFERENCES collections (id) ON DELETE CASCADE
        )
    )")) {
        qWarning() << "Failed to create requests table:" << query.lastError().text();
        return false;
    }
    
    // Create history table
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            method TEXT NOT NULL,
            url TEXT NOT NULL,
            headers TEXT,
            body TEXT,
            response TEXT,
            status_code INTEGER,
            response_time INTEGER,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )")) {
        qWarning() << "Failed to create history table:" << query.lastError().text();
        return false;
    }
    
    // Create default collection if none exists
    query.prepare("SELECT COUNT(*) FROM collections");
    if (query.exec() && query.next()) {
        if (query.value(0).toInt() == 0) {
            createCollection("My Collection", "Default collection for API requests");
        }
    }
    
    return true;
}

bool DatabaseManager::createCollection(const QString &name, const QString &description)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO collections (name, description) VALUES (?, ?)");
    query.addBindValue(name);
    query.addBindValue(description);
    
    if (!query.exec()) {
        qWarning() << "Failed to create collection:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteCollection(int collectionId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM collections WHERE id = ?");
    query.addBindValue(collectionId);
    
    if (!query.exec()) {
        qWarning() << "Failed to delete collection:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateCollection(int collectionId, const QString &name, const QString &description)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE collections SET name = ?, description = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(description);
    query.addBindValue(collectionId);
    
    if (!query.exec()) {
        qWarning() << "Failed to update collection:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::saveRequest(int collectionId, const QString &name, const QString &method,
                                const QString &url, const QString &headers, const QString &body,
                                const QString &parameters)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO requests (collection_id, name, method, url, headers, body, parameters) VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(collectionId);
    query.addBindValue(name);
    query.addBindValue(method);
    query.addBindValue(url);
    query.addBindValue(headers);
    query.addBindValue(body);
    query.addBindValue(parameters);
    
    if (!query.exec()) {
        qWarning() << "Failed to save request:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::deleteRequest(int requestId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM requests WHERE id = ?");
    query.addBindValue(requestId);
    
    if (!query.exec()) {
        qWarning() << "Failed to delete request:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateRequest(int requestId, const QString &name, const QString &method,
                                  const QString &url, const QString &headers, const QString &body,
                                  const QString &parameters)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE requests SET name = ?, method = ?, url = ?, headers = ?, body = ?, parameters = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(method);
    query.addBindValue(url);
    query.addBindValue(headers);
    query.addBindValue(body);
    query.addBindValue(parameters);
    query.addBindValue(requestId);
    
    if (!query.exec()) {
        qWarning() << "Failed to update request:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::saveToHistory(const QString &method, const QString &url, const QString &headers,
                                   const QString &body, const QString &response, int statusCode,
                                   qint64 responseTime)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO history (method, url, headers, body, response, status_code, response_time) VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(method);
    query.addBindValue(url);
    query.addBindValue(headers);
    query.addBindValue(body);
    query.addBindValue(response);
    query.addBindValue(statusCode);
    query.addBindValue(responseTime);
    
    if (!query.exec()) {
        qWarning() << "Failed to save to history:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::clearHistory()
{
    QSqlQuery query(m_database);
    if (!query.exec("DELETE FROM history")) {
        qWarning() << "Failed to clear history:" << query.lastError().text();
        return false;
    }
    
    return true;
}