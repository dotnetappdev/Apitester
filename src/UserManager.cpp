#include "UserManager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QDebug>
#include <QtCore/QCryptographicHash>
#include <QtCore/QRandomGenerator>
#include <QtCore/QDateTime>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>

UserManager::UserManager(QObject *parent)
    : QObject(parent)
    , m_currentUserId(-1)
{
    initializeUserDatabase();
}

UserManager::~UserManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool UserManager::initializeUserDatabase()
{
    // Get database path from settings or use default
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    QString dbFile = dbPath + "/apitester_users.db";
    
    m_database = QSqlDatabase::addDatabase("QSQLITE", "UserConnection");
    m_database.setDatabaseName(dbFile);
    
    if (!m_database.open()) {
        qDebug() << "Failed to open user database:" << m_database.lastError().text();
        return false;
    }
    
    bool success = createUserTables();
    if (success) {
        seedDefaultUsers();
    }
    
    return success;
}

bool UserManager::createUserTables()
{
    QSqlQuery query(m_database);
    
    // Create users table
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            salt TEXT NOT NULL,
            display_name TEXT NOT NULL,
            email TEXT,
            role INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_login_at DATETIME,
            is_active BOOLEAN DEFAULT 1
        )
    )";
    
    if (!query.exec(createUsersTable)) {
        qDebug() << "Failed to create users table:" << query.lastError().text();
        return false;
    }
    
    // Create user_collections table for user-specific collections
    QString createUserCollectionsTable = R"(
        CREATE TABLE IF NOT EXISTS user_collections (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            collection_id INTEGER NOT NULL,
            access_level INTEGER DEFAULT 0,
            assigned_by INTEGER,
            assigned_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(id),
            FOREIGN KEY (assigned_by) REFERENCES users(id)
        )
    )";
    
    if (!query.exec(createUserCollectionsTable)) {
        qDebug() << "Failed to create user_collections table:" << query.lastError().text();
        return false;
    }
    
    // Create user_sessions table for session management
    QString createSessionsTable = R"(
        CREATE TABLE IF NOT EXISTS user_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            session_token TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            expires_at DATETIME,
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )";
    
    if (!query.exec(createSessionsTable)) {
        qDebug() << "Failed to create sessions table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

void UserManager::seedDefaultUsers()
{
    // Check if admin user exists
    QSqlQuery checkQuery(m_database);
    checkQuery.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    checkQuery.addBindValue("admin");
    
    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() > 0) {
            return; // Admin already exists
        }
    }
    
    // Create default admin user
    registerUser("admin", "admin123", "Administrator", "admin@apitester.local", UserRole::Admin);
    
    // Create default test users
    registerUser("john_doe", "password123", "John Doe", "john@apitester.local", UserRole::User);
    registerUser("jane_smith", "password123", "Jane Smith", "jane@apitester.local", UserRole::User);
    registerUser("test_user", "test123", "Test User", "test@apitester.local", UserRole::User);
    
    qDebug() << "Seeded default users";
}

QString UserManager::hashPassword(const QString &password, const QString &salt) const
{
    // Use SHA-256 for password hashing (AES-256 would be for encryption, not hashing)
    QByteArray data = (password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hash.toHex();
}

QString UserManager::generateSalt() const
{
    QByteArray salt;
    for (int i = 0; i < 32; ++i) {
        salt.append(static_cast<char>(QRandomGenerator::global()->bounded(256)));
    }
    return salt.toHex();
}

bool UserManager::verifyPassword(const QString &password, const QString &hashedPassword, const QString &salt) const
{
    QString computedHash = hashPassword(password, salt);
    return computedHash == hashedPassword;
}

bool UserManager::registerUser(const QString &username, const QString &password, 
                              const QString &displayName, const QString &email, UserRole role)
{
    QString salt = generateSalt();
    QString hashedPassword = hashPassword(password, salt);
    
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO users (username, password_hash, salt, display_name, email, role)
        VALUES (?, ?, ?, ?, ?, ?)
    )");
    
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    query.addBindValue(salt);
    query.addBindValue(displayName);
    query.addBindValue(email);
    query.addBindValue(static_cast<int>(role));
    
    if (!query.exec()) {
        qDebug() << "Failed to register user:" << query.lastError().text();
        return false;
    }
    
    // Get the newly created user
    User newUser = getUserByUsername(username);
    emit userRegistered(newUser);
    
    return true;
}

bool UserManager::authenticateUser(const QString &username, const QString &password)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT id, password_hash, salt, is_active FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    int userId = query.value(0).toInt();
    QString storedHash = query.value(1).toString();
    QString salt = query.value(2).toString();
    bool isActive = query.value(3).toBool();
    
    if (!isActive) {
        return false;
    }
    
    if (verifyPassword(password, storedHash, salt)) {
        updateLastLogin(userId);
        return loginUser(username);
    }
    
    return false;
}

bool UserManager::loginUser(const QString &username)
{
    User user = getUserByUsername(username);
    if (user.id == -1) {
        return false;
    }
    
    m_currentUserId = user.id;
    m_currentUser = user;
    
    emit userLoggedIn(user);
    return true;
}

void UserManager::logoutUser()
{
    m_currentUserId = -1;
    m_currentUser = User{};
    emit userLoggedOut();
}

bool UserManager::isUserLoggedIn() const
{
    return m_currentUserId != -1;
}

User UserManager::getCurrentUser() const
{
    return m_currentUser;
}

bool UserManager::hasAdminAccess() const
{
    return isUserLoggedIn() && m_currentUser.role == UserRole::Admin;
}

bool UserManager::canAccessUser(int userId) const
{
    if (!isUserLoggedIn()) {
        return false;
    }
    
    // Admin can access all users, users can only access themselves
    return hasAdminAccess() || m_currentUserId == userId;
}

User UserManager::getUserByUsername(const QString &username) const
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT id, username, display_name, email, role, created_at, last_login_at, is_active
        FROM users WHERE username = ?
    )");
    query.addBindValue(username);
    
    if (query.exec() && query.next()) {
        User user;
        user.id = query.value(0).toInt();
        user.username = query.value(1).toString();
        user.displayName = query.value(2).toString();
        user.email = query.value(3).toString();
        user.role = static_cast<UserRole>(query.value(4).toInt());
        user.createdAt = query.value(5).toDateTime();
        user.lastLoginAt = query.value(6).toDateTime();
        user.isActive = query.value(7).toBool();
        return user;
    }
    
    return User{-1, "", "", "", UserRole::User, QDateTime(), QDateTime(), false};
}

User UserManager::getUserById(int userId) const
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT id, username, display_name, email, role, created_at, last_login_at, is_active
        FROM users WHERE id = ?
    )");
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        User user;
        user.id = query.value(0).toInt();
        user.username = query.value(1).toString();
        user.displayName = query.value(2).toString();
        user.email = query.value(3).toString();
        user.role = static_cast<UserRole>(query.value(4).toInt());
        user.createdAt = query.value(5).toDateTime();
        user.lastLoginAt = query.value(6).toDateTime();
        user.isActive = query.value(7).toBool();
        return user;
    }
    
    return User{-1, "", "", "", UserRole::User, QDateTime(), QDateTime(), false};
}

QList<User> UserManager::getAllUsers() const
{
    QList<User> users;
    
    if (!hasAdminAccess()) {
        // Non-admin users can only see themselves
        users.append(m_currentUser);
        return users;
    }
    
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT id, username, display_name, email, role, created_at, last_login_at, is_active
        FROM users ORDER BY display_name
    )");
    
    if (query.exec()) {
        while (query.next()) {
            User user;
            user.id = query.value(0).toInt();
            user.username = query.value(1).toString();
            user.displayName = query.value(2).toString();
            user.email = query.value(3).toString();
            user.role = static_cast<UserRole>(query.value(4).toInt());
            user.createdAt = query.value(5).toDateTime();
            user.lastLoginAt = query.value(6).toDateTime();
            user.isActive = query.value(7).toBool();
            users.append(user);
        }
    }
    
    return users;
}

bool UserManager::updateLastLogin(int userId)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE users SET last_login_at = CURRENT_TIMESTAMP WHERE id = ?");
    query.addBindValue(userId);
    return query.exec();
}

bool UserManager::changePassword(int userId, const QString &newPassword)
{
    if (!canAccessUser(userId)) {
        return false;
    }
    
    QString salt = generateSalt();
    QString hashedPassword = hashPassword(newPassword, salt);
    
    QSqlQuery query(m_database);
    query.prepare("UPDATE users SET password_hash = ?, salt = ? WHERE id = ?");
    query.addBindValue(hashedPassword);
    query.addBindValue(salt);
    query.addBindValue(userId);
    
    return query.exec();
}