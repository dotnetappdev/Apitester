#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QtCore/QObject>

class QApplication;

class ThemeManager : public QObject
{
    Q_OBJECT
    
public:
    enum Theme {
        Light,
        Dark
    };
    
    static ThemeManager& instance();
    
    void applyTheme(Theme theme);
    Theme currentTheme() const { return m_currentTheme; }
    
signals:
    void themeChanged(Theme theme);
    
private:
    explicit ThemeManager(QObject *parent = nullptr);
    void applyDarkTheme();
    void applyLightTheme();
    
    Theme m_currentTheme;
};

#endif // THEMEMANAGER_H