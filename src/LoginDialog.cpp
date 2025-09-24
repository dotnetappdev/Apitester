#include "LoginDialog.h"
#include "UserManager.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>

LoginDialog::LoginDialog(UserManager *userManager, QWidget *parent)
    : QDialog(parent)
    , m_userManager(userManager)
    , m_userCombo(nullptr)
    , m_passwordEdit(nullptr)
    , m_loginButton(nullptr)
    , m_createUserButton(nullptr)
    , m_statusLabel(nullptr)
{
    setWindowTitle("API Tester - User Login");
    setFixedSize(400, 320);
    setModal(true);
    
    setupUI();
    setupVisualStudioStyling();
    populateUsers();
}

void LoginDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Title area with Visual Studio style
    QHBoxLayout *titleLayout = new QHBoxLayout();
    
    m_userIcon = new QLabel();
    m_userIcon->setText("👤");
    m_userIcon->setStyleSheet("font-size: 24px; color: #0078d4;");
    titleLayout->addWidget(m_userIcon);
    
    m_titleLabel = new QLabel("Select User Profile");
    m_titleLabel->setStyleSheet(R"(
        font-size: 18px;
        font-weight: bold;
        color: #323130;
        margin-left: 10px;
    )");
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();
    
    mainLayout->addLayout(titleLayout);
    
    // Form layout
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    
    // User selection combo (Netflix-style profile switcher)
    m_userCombo = new QComboBox();
    m_userCombo->setMinimumHeight(35);
    connect(m_userCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LoginDialog::onUserSelectionChanged);
    formLayout->addRow("User Profile:", m_userCombo);
    
    // Password field
    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMinimumHeight(35);
    m_passwordEdit->setPlaceholderText("Enter password");
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    formLayout->addRow("Password:", m_passwordEdit);
    
    mainLayout->addLayout(formLayout);
    
    // Status label
    m_statusLabel = new QLabel();
    m_statusLabel->setStyleSheet("color: #d13438; font-size: 12px;");
    m_statusLabel->setWordWrap(true);
    m_statusLabel->hide();
    mainLayout->addWidget(m_statusLabel);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_createUserButton = new QPushButton("Create New User");
    m_createUserButton->setMinimumHeight(35);
    connect(m_createUserButton, &QPushButton::clicked, this, &LoginDialog::onCreateUserClicked);
    buttonLayout->addWidget(m_createUserButton);
    
    m_loginButton = new QPushButton("Login");
    m_loginButton->setMinimumHeight(35);
    m_loginButton->setMinimumWidth(100);
    m_loginButton->setDefault(true);
    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    buttonLayout->addWidget(m_loginButton);
    
    mainLayout->addLayout(buttonLayout);
}

void LoginDialog::setupVisualStudioStyling()
{
    // Visual Studio inspired styling
    setStyleSheet(R"(
        QDialog {
            background-color: #f3f2f1;
            border: 1px solid #d1d1d1;
        }
        
        QComboBox {
            border: 1px solid #d1d1d1;
            border-radius: 2px;
            padding: 8px 12px;
            background-color: white;
            font-size: 14px;
        }
        
        QComboBox:hover {
            border-color: #0078d4;
        }
        
        QComboBox:focus {
            border-color: #0078d4;
            outline: none;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 4px solid #323130;
        }
        
        QComboBox QAbstractItemView {
            border: 1px solid #d1d1d1;
            background-color: white;
            selection-background-color: #e3f2fd;
        }
        
        QLineEdit {
            border: 1px solid #d1d1d1;
            border-radius: 2px;
            padding: 8px 12px;
            background-color: white;
            font-size: 14px;
        }
        
        QLineEdit:hover {
            border-color: #0078d4;
        }
        
        QLineEdit:focus {
            border-color: #0078d4;
            outline: none;
        }
        
        QPushButton {
            background-color: #0078d4;
            color: white;
            border: none;
            border-radius: 2px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: 500;
        }
        
        QPushButton:hover {
            background-color: #106ebe;
        }
        
        QPushButton:pressed {
            background-color: #005a9e;
        }
        
        QPushButton:disabled {
            background-color: #f3f2f1;
            color: #a19f9d;
            border: 1px solid #edebe9;
        }
        
        QPushButton#createUserButton {
            background-color: #f3f2f1;
            color: #323130;
            border: 1px solid #d1d1d1;
        }
        
        QPushButton#createUserButton:hover {
            background-color: #edebe9;
        }
        
        QLabel {
            color: #323130;
        }
        
        QFormLayout QLabel {
            font-weight: 500;
            color: #323130;
        }
    )");
    
    // Set object names for specific styling
    m_createUserButton->setObjectName("createUserButton");
}

void LoginDialog::populateUsers()
{
    m_userCombo->clear();
    
    QList<User> users = m_userManager->getAllUsers();
    
    for (const User &user : users) {
        if (user.isActive) {
            QString displayText = QString("%1 (%2)")
                .arg(user.displayName)
                .arg(user.role == UserRole::Admin ? "Admin" : "User");
            
            m_userCombo->addItem(displayText, user.username);
        }
    }
    
    // Add default selection hint
    if (m_userCombo->count() == 0) {
        m_userCombo->addItem("No users available", "");
        m_loginButton->setEnabled(false);
    } else {
        // Select admin by default if available
        for (int i = 0; i < m_userCombo->count(); ++i) {
            QString username = m_userCombo->itemData(i).toString();
            User user = m_userManager->getUserByUsername(username);
            if (user.role == UserRole::Admin) {
                m_userCombo->setCurrentIndex(i);
                break;
            }
        }
    }
}

void LoginDialog::onUserSelectionChanged()
{
    m_statusLabel->hide();
    m_passwordEdit->clear();
    
    QString username = getSelectedUsername();
    if (username.isEmpty()) {
        m_loginButton->setEnabled(false);
    } else {
        m_loginButton->setEnabled(true);
        m_passwordEdit->setFocus();
    }
}

void LoginDialog::onLoginClicked()
{
    QString username = getSelectedUsername();
    QString password = getEnteredPassword();
    
    if (username.isEmpty()) {
        m_statusLabel->setText("Please select a user profile.");
        m_statusLabel->show();
        return;
    }
    
    if (password.isEmpty()) {
        m_statusLabel->setText("Please enter your password.");
        m_statusLabel->show();
        m_passwordEdit->setFocus();
        return;
    }
    
    if (m_userManager->authenticateUser(username, password)) {
        accept();
    } else {
        m_statusLabel->setText("Invalid username or password. Please try again.");
        m_statusLabel->show();
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
        
        // Shake effect
        QTimer::singleShot(100, this, [this]() {
            m_passwordEdit->setStyleSheet(m_passwordEdit->styleSheet() + 
                "border-color: #d13438; animation: shake 0.5s;");
        });
        
        QTimer::singleShot(1500, this, [this]() {
            m_passwordEdit->setStyleSheet("");
        });
    }
}

void LoginDialog::onCreateUserClicked()
{
    // For now, show information about default users
    QMessageBox::information(this, "Default Users", 
        "Default users created:\n\n"
        "• admin / admin123 (Admin)\n"
        "• john_doe / password123 (User)\n"
        "• jane_smith / password123 (User)\n"
        "• test_user / test123 (User)\n\n"
        "Admin users can create and manage other users through the application settings.");
}

QString LoginDialog::getSelectedUsername() const
{
    return m_userCombo->currentData().toString();
}

QString LoginDialog::getEnteredPassword() const
{
    return m_passwordEdit->text();
}