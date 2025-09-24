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
    QString visualStudioStyle = R"(
        /* Visual Studio Light Theme */
        QMainWindow {
            background-color: #f3f2f1;
            color: #323130;
        }
        
        QWidget {
            background-color: #f3f2f1;
            color: #323130;
            border: none;
        }
        
        QMenuBar {
            background-color: #f3f2f1;
            color: #323130;
            border-bottom: 1px solid #d1d1d1;
            padding: 2px;
        }
        
        QMenuBar::item {
            background: transparent;
            padding: 4px 8px;
            border-radius: 2px;
        }
        
        QMenuBar::item:selected {
            background-color: #edebe9;
        }
        
        QMenu {
            background-color: #ffffff;
            color: #323130;
            border: 1px solid #d1d1d1;
        }
        
        QMenu::item {
            padding: 6px 20px;
        }
        
        QMenu::item:selected {
            background-color: #e3f2fd;
        }
        
        QToolBar {
            background-color: #f3f2f1;
            border: none;
            spacing: 4px;
            padding: 4px;
        }
        
        QToolButton {
            background-color: transparent;
            border: none;
            padding: 6px;
            border-radius: 2px;
        }
        
        QToolButton:hover {
            background-color: #edebe9;
        }
        
        QToolButton:pressed {
            background-color: #e1dfdd;
        }
        
        QSplitter {
            background-color: #f3f2f1;
        }
        
        QSplitter::handle {
            background-color: #d1d1d1;
        }
        
        QSplitter::handle:horizontal {
            width: 4px;
        }
        
        QSplitter::handle:vertical {
            height: 4px;
        }
        
        QTabWidget::pane {
            border: 1px solid #d1d1d1;
            background-color: #ffffff;
        }
        
        QTabBar::tab {
            background-color: #edebe9;
            color: #323130;
            padding: 8px 16px;
            border: 1px solid #d1d1d1;
            border-bottom: none;
            border-top-left-radius: 2px;
            border-top-right-radius: 2px;
        }
        
        QTabBar::tab:selected {
            background-color: #ffffff;
            border-bottom: 1px solid #ffffff;
        }
        
        QTabBar::tab:hover:!selected {
            background-color: #e1dfdd;
        }
        
        QTreeView, QListView, QTableView {
            background-color: #ffffff;
            color: #323130;
            border: 1px solid #d1d1d1;
            gridline-color: #edebe9;
            selection-background-color: #e3f2fd;
        }
        
        QTreeView::item, QListView::item, QTableView::item {
            padding: 4px;
            border: none;
        }
        
        QTreeView::item:hover, QListView::item:hover, QTableView::item:hover {
            background-color: #f9f9f9;
        }
        
        QTreeView::item:selected, QListView::item:selected, QTableView::item:selected {
            background-color: #e3f2fd;
            color: #323130;
        }
        
        QHeaderView::section {
            background-color: #f3f2f1;
            color: #323130;
            padding: 6px;
            border: none;
            border-right: 1px solid #d1d1d1;
            border-bottom: 1px solid #d1d1d1;
        }
        
        QLineEdit, QTextEdit, QComboBox {
            background-color: #ffffff;
            color: #323130;
            border: 1px solid #d1d1d1;
            border-radius: 2px;
            padding: 6px;
        }
        
        QLineEdit:focus, QTextEdit:focus, QComboBox:focus {
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
            background-color: #ffffff;
            border: 1px solid #d1d1d1;
            selection-background-color: #e3f2fd;
        }
        
        QPushButton {
            background-color: #0078d4;
            color: #ffffff;
            border: none;
            border-radius: 2px;
            padding: 8px 16px;
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
        
        QStatusBar {
            background-color: #0078d4;
            color: #ffffff;
            border: none;
            padding: 2px;
        }
        
        QScrollBar:vertical {
            background: #f3f2f1;
            width: 16px;
            border: none;
        }
        
        QScrollBar::handle:vertical {
            background: #a19f9d;
            border-radius: 8px;
            min-height: 20px;
            margin: 2px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: #8a8886;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QScrollBar:horizontal {
            background: #f3f2f1;
            height: 16px;
            border: none;
        }
        
        QScrollBar::handle:horizontal {
            background: #a19f9d;
            border-radius: 8px;
            min-width: 20px;
            margin: 2px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background: #8a8886;
        }
        
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
        
        QProgressBar {
            border: 1px solid #d1d1d1;
            border-radius: 2px;
            text-align: center;
            background-color: #f3f2f1;
        }
        
        QProgressBar::chunk {
            background-color: #0078d4;
            border-radius: 1px;
        }
        
        QCheckBox {
            color: #323130;
            spacing: 6px;
        }
        
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #d1d1d1;
            border-radius: 2px;
            background-color: #ffffff;
        }
        
        QCheckBox::indicator:checked {
            background-color: #0078d4;
            border-color: #0078d4;
        }
        
        QCheckBox::indicator:checked:hover {
            background-color: #106ebe;
        }
        
        QGroupBox {
            font-weight: 500;
            color: #323130;
            border: 1px solid #d1d1d1;
            border-radius: 4px;
            margin-top: 8px;
            padding-top: 8px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px 0 4px;
            background-color: #f3f2f1;
        }
    )";
    
    qApp->setStyleSheet(visualStudioStyle);
}