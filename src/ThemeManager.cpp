#include "ThemeManager.h"
#include <QtWidgets/QApplication>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(Dark)
{
}

void ThemeManager::applyTheme(Theme theme)
{
    m_currentTheme = theme;
    
    if (theme == Dark) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }
    
    emit themeChanged(theme);
}

void ThemeManager::applyDarkTheme()
{
    QString darkStyle = R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        
        QWidget {
            background-color: #2b2b2b;
            color: #ffffff;
            border: none;
        }
        
        QMenuBar {
            background-color: #3c3c3c;
            color: #ffffff;
            border-bottom: 1px solid #555555;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        
        QMenuBar::item:selected {
            background-color: #555555;
        }
        
        QMenu {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
        }
        
        QMenu::item:selected {
            background-color: #555555;
        }
        
        QToolBar {
            background-color: #3c3c3c;
            border: none;
            spacing: 2px;
        }
        
        QToolButton {
            background-color: transparent;
            border: none;
            padding: 4px;
        }
        
        QToolButton:hover {
            background-color: #555555;
        }
        
        QTabWidget::pane {
            border: 1px solid #555555;
            background-color: #2b2b2b;
        }
        
        QTabBar::tab {
            background-color: #3c3c3c;
            color: #ffffff;
            padding: 8px 12px;
            border: 1px solid #555555;
            border-bottom: none;
        }
        
        QTabBar::tab:selected {
            background-color: #2b2b2b;
            border-bottom: 1px solid #2b2b2b;
        }
        
        QTabBar::tab:!selected {
            margin-top: 2px;
        }
        
        QPushButton {
            background-color: #4a4a4a;
            color: #ffffff;
            border: 1px solid #666666;
            padding: 6px 12px;
            border-radius: 3px;
        }
        
        QPushButton:hover {
            background-color: #555555;
        }
        
        QPushButton:pressed {
            background-color: #333333;
        }
        
        QLineEdit, QTextEdit, QComboBox {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #666666;
            padding: 4px;
            border-radius: 3px;
        }
        
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
            border-color: #0078d4;
        }
        
        QComboBox::drop-down {
            border: none;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 4px solid #ffffff;
        }
        
        QHeaderView::section {
            background-color: #3c3c3c;
            color: #ffffff;
            padding: 4px;
            border: 1px solid #555555;
        }
        
        QTreeView, QListView, QTableView {
            background-color: #2b2b2b;
            color: #ffffff;
            alternate-background-color: #3c3c3c;
        }
        
        QTreeView::item:selected, QListView::item:selected, QTableView::item:selected {
            background-color: #0078d4;
        }
        
        QScrollBar:vertical {
            background-color: #3c3c3c;
            width: 12px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:vertical {
            background-color: #666666;
            border-radius: 6px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: #888888;
        }
        
        QScrollBar:horizontal {
            background-color: #3c3c3c;
            height: 12px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: #666666;
            border-radius: 6px;
            min-width: 20px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: #888888;
        }
        
        QSplitter::handle {
            background-color: #555555;
        }
        
        QStatusBar {
            background-color: #3c3c3c;
            color: #ffffff;
            border-top: 1px solid #555555;
        }
    )";
    
    qApp->setStyleSheet(darkStyle);
}

void ThemeManager::applyLightTheme()
{
    QString lightStyle = R"(
        QMainWindow {
            background-color: #ffffff;
            color: #000000;
        }
        
        QWidget {
            background-color: #ffffff;
            color: #000000;
            border: none;
        }
        
        QMenuBar {
            background-color: #f0f0f0;
            color: #000000;
            border-bottom: 1px solid #cccccc;
        }
        
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        
        QMenuBar::item:selected {
            background-color: #e0e0e0;
        }
        
        QMenu {
            background-color: #ffffff;
            color: #000000;
            border: 1px solid #cccccc;
        }
        
        QMenu::item:selected {
            background-color: #e0e0e0;
        }
        
        QToolBar {
            background-color: #f0f0f0;
            border: none;
            spacing: 2px;
        }
        
        QToolButton {
            background-color: transparent;
            border: none;
            padding: 4px;
        }
        
        QToolButton:hover {
            background-color: #e0e0e0;
        }
        
        QTabWidget::pane {
            border: 1px solid #cccccc;
            background-color: #ffffff;
        }
        
        QTabBar::tab {
            background-color: #f0f0f0;
            color: #000000;
            padding: 8px 12px;
            border: 1px solid #cccccc;
            border-bottom: none;
        }
        
        QTabBar::tab:selected {
            background-color: #ffffff;
            border-bottom: 1px solid #ffffff;
        }
        
        QTabBar::tab:!selected {
            margin-top: 2px;
        }
        
        QPushButton {
            background-color: #f8f8f8;
            color: #000000;
            border: 1px solid #cccccc;
            padding: 6px 12px;
            border-radius: 3px;
        }
        
        QPushButton:hover {
            background-color: #e8e8e8;
        }
        
        QPushButton:pressed {
            background-color: #d8d8d8;
        }
        
        QLineEdit, QTextEdit, QComboBox {
            background-color: #ffffff;
            color: #000000;
            border: 1px solid #cccccc;
            padding: 4px;
            border-radius: 3px;
        }
        
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
            border-color: #0078d4;
        }
        
        QComboBox::drop-down {
            border: none;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 4px solid #000000;
        }
        
        QHeaderView::section {
            background-color: #f0f0f0;
            color: #000000;
            padding: 4px;
            border: 1px solid #cccccc;
        }
        
        QTreeView, QListView, QTableView {
            background-color: #ffffff;
            color: #000000;
            alternate-background-color: #f8f8f8;
        }
        
        QTreeView::item:selected, QListView::item:selected, QTableView::item:selected {
            background-color: #0078d4;
            color: #ffffff;
        }
        
        QScrollBar:vertical {
            background-color: #f0f0f0;
            width: 12px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:vertical {
            background-color: #cccccc;
            border-radius: 6px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: #aaaaaa;
        }
        
        QScrollBar:horizontal {
            background-color: #f0f0f0;
            height: 12px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: #cccccc;
            border-radius: 6px;
            min-width: 20px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: #aaaaaa;
        }
        
        QSplitter::handle {
            background-color: #cccccc;
        }
        
        QStatusBar {
            background-color: #f0f0f0;
            color: #000000;
            border-top: 1px solid #cccccc;
        }
    )";
    
    qApp->setStyleSheet(lightStyle);
}