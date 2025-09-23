#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>

class RequestPanel;
class ResponsePanel;
class CollectionManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newRequest();
    void saveCollection();
    void loadCollection();
    void toggleTheme();
    void about();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void createNewRequestTab();

    // UI Components
    QSplitter *m_mainSplitter;
    QSplitter *m_leftSplitter;
    QTabWidget *m_requestTabs;
    CollectionManager *m_collectionManager;
    ResponsePanel *m_responsePanel;
    
    // Actions
    QAction *m_newRequestAction;
    QAction *m_saveAction;
    QAction *m_loadAction;
    QAction *m_toggleThemeAction;
    QAction *m_aboutAction;
    QAction *m_exitAction;
};

#endif // MAINWINDOW_H