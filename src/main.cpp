#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include "MainWindow.h"
#include "DatabaseManager.h"
#include "ThemeManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("API Tester");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ApiTester");
    app.setOrganizationDomain("apitester.com");
    
    // Initialize database
    DatabaseManager::instance().initialize();
    
    // Initialize theme manager and apply default theme
    ThemeManager::instance().applyTheme(ThemeManager::Dark);
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}