cmake_minimum_required(VERSION 3.16)
project(ApiTester VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt components
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Sql)

# Set up Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/RequestPanel.cpp
    src/ResponsePanel.cpp
    src/CollectionManager.cpp
    src/DatabaseManager.cpp
    src/ThemeManager.cpp
    src/HttpClient.cpp
    src/RequestModel.cpp
    src/CollectionModel.cpp
    src/SettingsDialog.cpp
    src/TestRunner.cpp
    src/TestExplorer.cpp
    src/SyntaxHighlighter.cpp
    src/UserManager.cpp
    src/LoginDialog.cpp
    src/ConfigManager.cpp
)

set(HEADERS
    src/MainWindow.h
    src/RequestPanel.h
    src/ResponsePanel.h
    src/CollectionManager.h
    src/DatabaseManager.h
    src/ThemeManager.h
    src/HttpClient.h
    src/RequestModel.h
    src/CollectionModel.h
    src/SettingsDialog.h
    src/TestRunner.h
    src/TestExplorer.h
    src/SyntaxHighlighter.h
    src/UserManager.h
    src/LoginDialog.h
    src/ConfigManager.h
)

# UI files
# set(UI_FILES
#     src/MainWindow.ui
#     src/RequestPanel.ui
#     src/ResponsePanel.ui
# )

# Resource files
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(ApiTester ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt libraries
target_link_libraries(ApiTester
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Sql
)

# Set target properties
set_target_properties(ApiTester PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)