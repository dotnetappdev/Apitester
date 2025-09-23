#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include "MainWindow.h"
#include "DatabaseManager.h"
#include "ThemeManager.h"
#include "UserManager.h"
#include "LoginDialog.h"
#include "ConfigManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("API Tester");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ApiTester");
    app.setOrganizationDomain("apitester.com");
    
    // Initialize configuration
    ConfigManager *config = ConfigManager::instance();
    
    // Initialize database
    DatabaseManager::instance().initialize();
    
    // Initialize user manager
    UserManager userManager;
    
    // Check if multi-user mode is enabled
    if (config->isMultiUserEnabled()) {
        // Show login dialog
        LoginDialog loginDialog(&userManager);
        if (loginDialog.exec() != QDialog::Accepted) {
            return 0; // User cancelled login
        }
        
        // Save current user in config
        config->setCurrentUser(userManager.getCurrentUser().username);
        config->sync();
    } else {
        // Single-user mode, login as admin automatically
        if (!userManager.authenticateUser("admin", "admin123")) {
            QMessageBox::critical(nullptr, "Error", 
                "Failed to initialize single-user mode. Please check the application installation.");
            return 1;
        }
    }
    
    // Apply theme from configuration
    QString theme = config->getTheme();
    if (theme == "light") {
        ThemeManager::instance().applyTheme(ThemeManager::Light);
    } else {
        ThemeManager::instance().applyTheme(ThemeManager::Dark);
    }
    
    // Create main window
    MainWindow window;
    
    // Pass user manager to main window for user-specific functionality
    window.setUserManager(&userManager);
    
    // Restore window geometry if available
    QByteArray geometry = config->getWindowGeometry();
    if (!geometry.isEmpty()) {
        window.restoreGeometry(geometry);
    } else {
        window.resize(1400, 900);
        window.move(100, 100);
    }
    
    // Restore window state if available
    QByteArray state = config->getWindowState();
    if (!state.isEmpty()) {
        window.restoreState(state);
    }
    
    window.show();
    
    int result = app.exec();
    
    // Save window geometry and state
    config->saveWindowGeometry(window.saveGeometry());
    config->saveWindowState(window.saveState());
    config->sync();
    
    return result;
}