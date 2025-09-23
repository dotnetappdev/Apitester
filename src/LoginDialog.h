#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

class UserManager;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(UserManager *userManager, QWidget *parent = nullptr);

    QString getSelectedUsername() const;
    QString getEnteredPassword() const;

private slots:
    void onLoginClicked();
    void onUserSelectionChanged();
    void onCreateUserClicked();

private:
    void setupUI();
    void setupVisualStudioStyling();
    void populateUsers();

    UserManager *m_userManager;
    QComboBox *m_userCombo;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QPushButton *m_createUserButton;
    QLabel *m_statusLabel;
    QLabel *m_titleLabel;
    QLabel *m_userIcon;
};

#endif // LOGINDIALOG_H