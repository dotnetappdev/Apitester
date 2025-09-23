#include "ConfigManager.h"
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>

ConfigManager* ConfigManager::s_instance = nullptr;

ConfigManager* ConfigManager::instance()
{
    if (!s_instance) {
        s_instance = new ConfigManager();
    }
    return s_instance;
}

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    // Create config directory if it doesn't exist
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configPath);
    
    // Use INI format for easier editing
    QString configFile = configPath + "/apitester.ini";
    m_settings = new QSettings(configFile, QSettings::IniFormat, this);
    
    // Set default values if not present
    if (!m_settings->contains("Database/Path")) {
        QString defaultDbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        m_settings->setValue("Database/Path", defaultDbPath);
    }
    
    if (!m_settings->contains("Application/Theme")) {
        m_settings->setValue("Application/Theme", "dark");
    }
    
    if (!m_settings->contains("MultiUser/Enabled")) {
        m_settings->setValue("MultiUser/Enabled", true);
    }
    
    if (!m_settings->contains("Network/Host")) {
        m_settings->setValue("Network/Host", "localhost");
    }
    
    if (!m_settings->contains("Network/Port")) {
        m_settings->setValue("Network/Port", 5432);
    }
    
    if (!m_settings->contains("Database/NetworkEnabled")) {
        m_settings->setValue("Database/NetworkEnabled", false);
    }
}

QString ConfigManager::getDatabasePath() const
{
    return m_settings->value("Database/Path").toString();
}

void ConfigManager::setDatabasePath(const QString &path)
{
    m_settings->setValue("Database/Path", path);
}

bool ConfigManager::isNetworkDatabase() const
{
    return m_settings->value("Database/NetworkEnabled", false).toBool();
}

void ConfigManager::setNetworkDatabase(bool enabled)
{
    m_settings->setValue("Database/NetworkEnabled", enabled);
}

QString ConfigManager::getTheme() const
{
    return m_settings->value("Application/Theme", "dark").toString();
}

void ConfigManager::setTheme(const QString &theme)
{
    m_settings->setValue("Application/Theme", theme);
}

bool ConfigManager::isMultiUserEnabled() const
{
    return m_settings->value("MultiUser/Enabled", true).toBool();
}

void ConfigManager::setMultiUserEnabled(bool enabled)
{
    m_settings->setValue("MultiUser/Enabled", enabled);
}

QString ConfigManager::getCurrentUser() const
{
    return m_settings->value("MultiUser/CurrentUser").toString();
}

void ConfigManager::setCurrentUser(const QString &username)
{
    m_settings->setValue("MultiUser/CurrentUser", username);
}

void ConfigManager::saveWindowGeometry(const QByteArray &geometry)
{
    m_settings->setValue("Window/Geometry", geometry);
}

QByteArray ConfigManager::getWindowGeometry() const
{
    return m_settings->value("Window/Geometry").toByteArray();
}

void ConfigManager::saveWindowState(const QByteArray &state)
{
    m_settings->setValue("Window/State", state);
}

QByteArray ConfigManager::getWindowState() const
{
    return m_settings->value("Window/State").toByteArray();
}

QString ConfigManager::getNetworkHost() const
{
    return m_settings->value("Network/Host", "localhost").toString();
}

void ConfigManager::setNetworkHost(const QString &host)
{
    m_settings->setValue("Network/Host", host);
}

int ConfigManager::getNetworkPort() const
{
    return m_settings->value("Network/Port", 5432).toInt();
}

void ConfigManager::setNetworkPort(int port)
{
    m_settings->setValue("Network/Port", port);
}

void ConfigManager::sync()
{
    m_settings->sync();
}