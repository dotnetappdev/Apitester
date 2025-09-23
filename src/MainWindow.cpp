#include "MainWindow.h"
#include "RequestPanel.h"
#include "ResponsePanel.h"
#include "CollectionManager.h"
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
    , m_leftSplitter(nullptr)
    , m_requestTabs(nullptr)
    , m_collectionManager(nullptr)
    , m_responsePanel(nullptr)
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
    
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    centralLayout->addWidget(m_mainSplitter);
    
    // Create left splitter for collections and requests
    m_leftSplitter = new QSplitter(Qt::Vertical, this);
    
    // Create collection manager
    m_collectionManager = new CollectionManager(this);
    m_leftSplitter->addWidget(m_collectionManager);
    
    // Create request tabs
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
    
    m_leftSplitter->addWidget(m_requestTabs);
    
    // Create response panel
    m_responsePanel = new ResponsePanel(this);
    
    // Add to main splitter
    m_mainSplitter->addWidget(m_leftSplitter);
    m_mainSplitter->addWidget(m_responsePanel);
    
    // Set splitter sizes
    m_leftSplitter->setSizes({200, 600});
    m_mainSplitter->setSizes({800, 600});
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
        "<li>Dark and light themes</li>"
        "<li>SQLite database for storage</li>"
        "</ul>"
        "<p>Copyright © 2024 API Tester</p>");
}