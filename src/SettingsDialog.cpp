#include "SettingsDialog.h"
#include <QtCore/QStandardPaths>
#include <QtCore/QSettings>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");
    setModal(true);
    resize(500, 400);
    
    setupUI();
    
    // Load current settings
    QSettings settings;
    setDatabasePath(settings.value("database/path", QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).toString());
    setTheme(settings.value("appearance/theme", "Dark").toString());
    
    // Store original values for cancel functionality
    m_originalDatabasePath = getDatabasePath();
    m_originalTheme = getTheme();
}

void SettingsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    m_tabWidget = new QTabWidget();
    mainLayout->addWidget(m_tabWidget);
    
    setupGeneralTab();
    setupAppearanceTab();
    setupDatabaseTab();
    
    // Dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::onRejected);
    mainLayout->addWidget(buttonBox);
}

void SettingsDialog::setupGeneralTab()
{
    m_generalTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_generalTab);
    
    QLabel *infoLabel = new QLabel("General application settings will be available in future versions.");
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    
    layout->addStretch();
    
    m_tabWidget->addTab(m_generalTab, "General");
}

void SettingsDialog::setupAppearanceTab()
{
    m_appearanceTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_appearanceTab);
    
    // Theme selection group
    QGroupBox *themeGroup = new QGroupBox("Theme");
    QFormLayout *themeLayout = new QFormLayout(themeGroup);
    
    m_themeCombo = new QComboBox();
    m_themeCombo->addItems({"Dark", "Light"});
    themeLayout->addRow("Application Theme:", m_themeCombo);
    
    QLabel *themeInfo = new QLabel("Select your preferred application theme. Changes will be applied immediately.");
    themeInfo->setWordWrap(true);
    themeInfo->setStyleSheet("color: gray; font-size: 11px;");
    themeLayout->addRow(themeInfo);
    
    layout->addWidget(themeGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_appearanceTab, "Appearance");
}

void SettingsDialog::setupDatabaseTab()
{
    m_databaseTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_databaseTab);
    
    // Database path group
    QGroupBox *pathGroup = new QGroupBox("Database Location");
    QVBoxLayout *pathLayout = new QVBoxLayout(pathGroup);
    
    QFormLayout *pathFormLayout = new QFormLayout();
    
    QHBoxLayout *pathEditLayout = new QHBoxLayout();
    m_databasePathEdit = new QLineEdit();
    m_databasePathEdit->setReadOnly(true);
    m_browseButton = new QPushButton("Browse...");
    connect(m_browseButton, &QPushButton::clicked, this, &SettingsDialog::browseDatabasePath);
    
    pathEditLayout->addWidget(m_databasePathEdit);
    pathEditLayout->addWidget(m_browseButton);
    
    pathFormLayout->addRow("Database Path:", pathEditLayout);
    
    m_resetPathButton = new QPushButton("Reset to Default");
    connect(m_resetPathButton, &QPushButton::clicked, this, &SettingsDialog::resetDatabasePath);
    pathFormLayout->addRow("", m_resetPathButton);
    
    pathLayout->addLayout(pathFormLayout);
    
    m_databaseInfoLabel = new QLabel();
    m_databaseInfoLabel->setWordWrap(true);
    m_databaseInfoLabel->setStyleSheet("color: gray; font-size: 11px;");
    pathLayout->addWidget(m_databaseInfoLabel);
    
    layout->addWidget(pathGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_databaseTab, "Database");
}

QString SettingsDialog::getDatabasePath() const
{
    return m_databasePathEdit->text();
}

QString SettingsDialog::getTheme() const
{
    return m_themeCombo->currentText();
}

void SettingsDialog::setDatabasePath(const QString &path)
{
    m_databasePathEdit->setText(path);
    updateDatabaseInfo();
}

void SettingsDialog::setTheme(const QString &theme)
{
    int index = m_themeCombo->findText(theme);
    if (index >= 0) {
        m_themeCombo->setCurrentIndex(index);
    }
}

void SettingsDialog::browseDatabasePath()
{
    QString currentPath = m_databasePathEdit->text();
    QDir dir(currentPath);
    if (!dir.exists()) {
        currentPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    
    QString selectedPath = QFileDialog::getExistingDirectory(this, 
        "Select Database Directory", currentPath);
    
    if (!selectedPath.isEmpty()) {
        setDatabasePath(selectedPath);
    }
}

void SettingsDialog::resetDatabasePath()
{
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    setDatabasePath(defaultPath);
}

void SettingsDialog::updateDatabaseInfo()
{
    QString path = getDatabasePath();
    QDir dir(path);
    
    if (dir.exists()) {
        m_databaseInfoLabel->setText(QString("Database will be stored in: %1/apitester.db").arg(path));
    } else {
        m_databaseInfoLabel->setText(QString("Directory will be created: %1/apitester.db").arg(path));
    }
}

void SettingsDialog::onAccepted()
{
    // Save settings
    QSettings settings;
    settings.setValue("database/path", getDatabasePath());
    settings.setValue("appearance/theme", getTheme());
    
    // Emit signals if values changed
    if (getDatabasePath() != m_originalDatabasePath) {
        emit databasePathChanged(getDatabasePath());
    }
    
    if (getTheme() != m_originalTheme) {
        emit themeChanged(getTheme());
    }
    
    accept();
}

void SettingsDialog::onRejected()
{
    // Restore original values
    setDatabasePath(m_originalDatabasePath);
    setTheme(m_originalTheme);
    
    reject();
}