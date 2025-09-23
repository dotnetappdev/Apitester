#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGroupBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    
    // Getters
    QString getDatabasePath() const;
    QString getTheme() const;
    
    // Setters
    void setDatabasePath(const QString &path);
    void setTheme(const QString &theme);
    
signals:
    void databasePathChanged(const QString &newPath);
    void themeChanged(const QString &theme);
    
private slots:
    void browseDatabasePath();
    void resetDatabasePath();
    void onAccepted();
    void onRejected();
    void updateDatabaseInfo();
    
private:
    void setupUI();
    void setupGeneralTab();
    void setupAppearanceTab();
    void setupDatabaseTab();
    
    // UI Components
    QTabWidget *m_tabWidget;
    
    // General tab
    QWidget *m_generalTab;
    
    // Appearance tab
    QWidget *m_appearanceTab;
    QComboBox *m_themeCombo;
    
    // Database tab
    QWidget *m_databaseTab;
    QLineEdit *m_databasePathEdit;
    QPushButton *m_browseButton;
    QPushButton *m_resetPathButton;
    QLabel *m_databaseInfoLabel;
    
    // Original values for cancel functionality
    QString m_originalDatabasePath;
    QString m_originalTheme;
};

#endif // SETTINGSDIALOG_H