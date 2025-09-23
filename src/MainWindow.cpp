#include "MainWindow.h"
#include "RequestPanel.h"
#include "ResponsePanel.h"
#include "CollectionManager.h"
#include "TestExplorer.h"
#include "SettingsDialog.h"
#include "ThemeManager.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mainSplitter(nullptr)
    , m_rightSplitter(nullptr)
    , m_requestTabs(nullptr)
    , m_collectionManager(nullptr)
    , m_responsePanel(nullptr)
    , m_testExplorer(nullptr)
{
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    
    // Create the first request tab
    createNewRequestTab();
    
    // Set window properties
    setWindowTitle("API Tester v1.0");
    setMinimumSize(1200, 800);
    resize(1400, 900);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Create central widget and main splitter
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout *centralLayout = new QHBoxLayout(centralWidget);
    centralLayout->setContentsMargins(5, 5, 5, 5);
    
    // Main horizontal splitter for Postman-like layout: [Collections | Request+Response]
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    centralLayout->addWidget(m_mainSplitter);
    
    // Left panel: Collections with integrated test explorer
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(5);
    
    // Collections manager
    m_collectionManager = new CollectionManager(this);
    leftLayout->addWidget(m_collectionManager, 2); // Give more space to collections
    
    // Test explorer (smaller section at bottom)
    m_testExplorer = new TestExplorer(this);
    leftLayout->addWidget(m_testExplorer, 1);
    
    // Add left panel to main splitter
    m_mainSplitter->addWidget(leftPanel);
    
    // Right side: Request tabs and Response panel
    m_rightSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Request tabs area
    QWidget *requestArea = new QWidget();
    QVBoxLayout *requestLayout = new QVBoxLayout(requestArea);
    requestLayout->setContentsMargins(0, 0, 0, 0);
    
    m_requestTabs = new QTabWidget(this);
    m_requestTabs->setTabsClosable(true);
    m_requestTabs->setMovable(true);
    connect(m_requestTabs, &QTabWidget::tabCloseRequested, [this](int index) {
        if (m_requestTabs->count() > 1) {
            QWidget *tab = m_requestTabs->widget(index);
            m_requestTabs->removeTab(index);
            tab->deleteLater();
        }
    });
    
    requestLayout->addWidget(m_requestTabs);
    
    // Response panel
    m_responsePanel = new ResponsePanel(this);
    
    // Add request area and response panel to right splitter
    m_rightSplitter->addWidget(requestArea);
    m_rightSplitter->addWidget(m_responsePanel);
    
    // Add right splitter to main splitter
    m_mainSplitter->addWidget(m_rightSplitter);
    
    // Set splitter sizes for Postman-like layout
    // Collections panel: 250px, Request+Response: remaining space
    m_mainSplitter->setSizes({250, 1150});
    
    // Request area: 600px, Response: 550px
    m_rightSplitter->setSizes({600, 550});
}

void MainWindow::setupMenuBar()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    m_newRequestAction = new QAction("&New Request", this);
    m_newRequestAction->setShortcut(QKeySequence::New);
    connect(m_newRequestAction, &QAction::triggered, this, &MainWindow::newRequest);
    fileMenu->addAction(m_newRequestAction);
    
    fileMenu->addSeparator();
    
    m_saveAction = new QAction("&Save Collection", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveCollection);
    fileMenu->addAction(m_saveAction);
    
    m_loadAction = new QAction("&Load Collection", this);
    m_loadAction->setShortcut(QKeySequence::Open);
    connect(m_loadAction, &QAction::triggered, this, &MainWindow::loadCollection);
    fileMenu->addAction(m_loadAction);
    
    fileMenu->addSeparator();
    
    m_settingsAction = new QAction("&Settings", this);
    m_settingsAction->setShortcut(QKeySequence::Preferences);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::showSettings);
    fileMenu->addAction(m_settingsAction);
    
    fileMenu->addSeparator();
    
    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(m_exitAction);
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    
    m_toggleThemeAction = new QAction("Toggle &Theme", this);
    m_toggleThemeAction->setShortcut(QKeySequence("Ctrl+T"));
    connect(m_toggleThemeAction, &QAction::triggered, this, &MainWindow::toggleTheme);
    viewMenu->addAction(m_toggleThemeAction);
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    
    m_aboutAction = new QAction("&About", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupToolBar()
{
    QToolBar *mainToolBar = addToolBar("Main");
    mainToolBar->addAction(m_newRequestAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(m_saveAction);
    mainToolBar->addAction(m_loadAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(m_toggleThemeAction);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready");
}

void MainWindow::createNewRequestTab()
{
    RequestPanel *requestPanel = new RequestPanel(this);
    
    // Connect request panel to response panel
    connect(requestPanel, &RequestPanel::responseReceived, m_responsePanel, &ResponsePanel::displayResponse);
    connect(requestPanel, &RequestPanel::requestStarted, [this]() {
        statusBar()->showMessage("Sending request...");
    });
    connect(requestPanel, &RequestPanel::requestFinished, [this]() {
        statusBar()->showMessage("Request completed");
    });
    
    // Connect collection manager to request panel for loading saved requests
    connect(m_collectionManager, &CollectionManager::requestSelected,
            [requestPanel](const QString &name, const QString &method, const QString &url,
                          const QString &headers, const QString &body, const QString &parameters) {
        requestPanel->setMethod(method);
        requestPanel->setUrl(url);
        requestPanel->setHeaders(headers);
        requestPanel->setBody(body);
        requestPanel->setParameters(parameters);
    });
    
    int tabIndex = m_requestTabs->addTab(requestPanel, "New Request");
    m_requestTabs->setCurrentIndex(tabIndex);
}

void MainWindow::newRequest()
{
    createNewRequestTab();
}

void MainWindow::saveCollection()
{
    // TODO: Implement save collection functionality
    statusBar()->showMessage("Save collection - Not yet implemented", 2000);
}

void MainWindow::loadCollection()
{
    // TODO: Implement load collection functionality
    statusBar()->showMessage("Load collection - Not yet implemented", 2000);
}

void MainWindow::toggleTheme()
{
    ThemeManager &themeManager = ThemeManager::instance();
    ThemeManager::Theme currentTheme = themeManager.currentTheme();
    
    if (currentTheme == ThemeManager::Dark) {
        themeManager.applyTheme(ThemeManager::Light);
    } else {
        themeManager.applyTheme(ThemeManager::Dark);
    }
    
    statusBar()->showMessage(QString("Switched to %1 theme").arg(
        themeManager.currentTheme() == ThemeManager::Dark ? "dark" : "light"), 2000);
}

void MainWindow::about()
{
    QMessageBox::about(this, "About API Tester",
        "<h3>API Tester v1.0</h3>"
        "<p>A modern Postman-like API testing application built with Qt and C++.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Multiple HTTP methods (GET, POST, PUT, DELETE, etc.)</li>"
        "<li>Custom headers and authentication</li>"
        "<li>Request body support (JSON, raw, form-data)</li>"
        "<li>Response viewer with syntax highlighting</li>"
        "<li>Collections management</li>"
        "<li>Test runner with pass/fail indicators</li>"
        "<li>Dark and light themes</li>"
        "<li>SQLite database for storage</li>"
        "</ul>"
        "<p>Copyright © 2024 API Tester</p>");
}

void MainWindow::showSettings()
{
    SettingsDialog dialog(this);
    
    // Connect settings change signals
    connect(&dialog, &SettingsDialog::databasePathChanged, this, &MainWindow::onDatabasePathChanged);
    connect(&dialog, &SettingsDialog::themeChanged, this, &MainWindow::onThemeChanged);
    
    dialog.exec();
}

void MainWindow::onDatabasePathChanged(const QString &newPath)
{
    // TODO: Implement database migration to new path
    statusBar()->showMessage(QString("Database path changed to: %1 (restart required)").arg(newPath), 5000);
}

void MainWindow::onThemeChanged(const QString &theme)
{
    ThemeManager &themeManager = ThemeManager::instance();
    
    if (theme == "Dark") {
        themeManager.applyTheme(ThemeManager::Dark);
    } else {
        themeManager.applyTheme(ThemeManager::Light);
    }
    
    statusBar()->showMessage(QString("Switched to %1 theme").arg(theme.toLower()), 2000);
}