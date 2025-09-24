#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

class RequestPanel;
class ResponsePanel;
class CollectionManager;
class TestExplorer;
class UserManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void setUserManager(UserManager *userManager);

private slots:
    void newRequest();
    void saveCollection();
    void loadCollection();
    void showSettings();
    void toggleTheme();
    void about();
    void onDatabasePathChanged(const QString &newPath);
    void onThemeChanged(const QString &theme);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void createNewRequestTab();
    void updateUserInterface();
    void showUserProfile();

    // UI Components
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;  // For requests and response
    QTabWidget *m_requestTabs;
    CollectionManager *m_collectionManager;
    ResponsePanel *m_responsePanel;
    TestExplorer *m_testExplorer;
    
    // User management
    UserManager *m_userManager;
    QLabel *m_userLabel;
    QPushButton *m_userProfileButton;
    
    // Actions
    QAction *m_newRequestAction;
    QAction *m_saveAction;
    QAction *m_loadAction;
    QAction *m_settingsAction;
    QAction *m_toggleThemeAction;
    QAction *m_aboutAction;
    QAction *m_exitAction;
};

#endif // MAINWINDOW_H