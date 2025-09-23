#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>

class QSqlQuery;

enum class UserRole {
    User,
    Admin
};

struct User {
    int id;
    QString username;
    QString displayName;
    QString email;
    UserRole role;
    QDateTime createdAt;
    QDateTime lastLoginAt;
    bool isActive;
};

class UserManager : public QObject
{
    Q_OBJECT

public:
    explicit UserManager(QObject *parent = nullptr);
    ~UserManager();

    // Authentication
    bool authenticateUser(const QString &username, const QString &password);
    bool registerUser(const QString &username, const QString &password, 
                     const QString &displayName, const QString &email, UserRole role = UserRole::User);
    
    // User management
    User getCurrentUser() const;
    QList<User> getAllUsers() const;
    bool updateUser(const User &user);
    bool deleteUser(int userId);
    bool changePassword(int userId, const QString &newPassword);
    
    // Session management
    bool loginUser(const QString &username);
    void logoutUser();
    bool isUserLoggedIn() const;
    
    // Role management
    bool hasAdminAccess() const;
    bool canAccessUser(int userId) const;
    
    // Public user access methods
    User getUserByUsername(const QString &username) const;
    User getUserById(int userId) const;
    
    // Database initialization
    bool initializeUserDatabase();
    void seedDefaultUsers();

signals:
    void userLoggedIn(const User &user);
    void userLoggedOut();
    void userRegistered(const User &user);

private:
    // Password hashing using AES-256
    QString hashPassword(const QString &password, const QString &salt) const;
    QString generateSalt() const;
    bool verifyPassword(const QString &password, const QString &hashedPassword, const QString &salt) const;
    
    // Database operations
    bool createUserTables();
    bool updateLastLogin(int userId);
    
    int m_currentUserId;
    User m_currentUser;
    QSqlDatabase m_database;
};

#endif // USERMANAGER_H