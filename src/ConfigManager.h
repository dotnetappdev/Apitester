#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSettings>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager* instance();
    
    // Database settings
    QString getDatabasePath() const;
    void setDatabasePath(const QString &path);
    bool isNetworkDatabase() const;
    void setNetworkDatabase(bool enabled);
    
    // Application settings
    QString getTheme() const;
    void setTheme(const QString &theme);
    
    // Multi-user settings
    bool isMultiUserEnabled() const;
    void setMultiUserEnabled(bool enabled);
    QString getCurrentUser() const;
    void setCurrentUser(const QString &username);
    
    // Window settings
    void saveWindowGeometry(const QByteArray &geometry);
    QByteArray getWindowGeometry() const;
    void saveWindowState(const QByteArray &state);
    QByteArray getWindowState() const;
    
    // Network settings
    QString getNetworkHost() const;
    void setNetworkHost(const QString &host);
    int getNetworkPort() const;
    void setNetworkPort(int port);
    
    // Sync settings
    void sync();

private:
    explicit ConfigManager(QObject *parent = nullptr);
    static ConfigManager *s_instance;
    QSettings *m_settings;
};

#endif // CONFIGMANAGER_H