# Visual Studio Setup Guide for ApiTester

This guide helps you build the ApiTester application using Visual Studio on Windows.

## Prerequisites

1. **Visual Studio 2019 or 2022** with C++ development tools
2. **Qt5 for MSVC** (Qt 5.15.x recommended)
3. **Windows SDK 10.0** or later

## Qt5 Installation

### Option 1: Qt Installer (Recommended)
1. Download Qt installer from [qt.io](https://www.qt.io/download-qt-installer)
2. Install Qt 5.15.x with MSVC 2019 64-bit compiler
3. Make sure to install the following components:
   - Qt 5.15.x
   - MSVC 2019 64-bit
   - Qt Network
   - Qt SQL
   - Qt Widgets

### Option 2: vcpkg
```cmd
vcpkg install qt5:x64-windows
```

## Environment Setup

1. Set the `QTDIR` environment variable to your Qt installation path:
   - Example: `C:\Qt\5.15.2\msvc2019_64`
   - Add this to your system environment variables

2. Add Qt's bin directory to your PATH:
   - Example: `C:\Qt\5.15.2\msvc2019_64\bin`

## Building in Visual Studio

1. Open `ApiTester.sln` in Visual Studio
2. Select your desired configuration (Debug/Release) and platform (x64)
3. Build the solution (Ctrl+Shift+B)

The executable will be created in:
- Debug: `build\Debug\ApiTester.exe`
- Release: `build\Release\ApiTester.exe`

## Configuration Notes

- The project is configured for x64 builds only
- Qt libraries are linked dynamically
- Make sure Qt DLLs are accessible when running the application

## Troubleshooting

### Qt Headers Not Found
- Verify `QTDIR` environment variable is set correctly
- Restart Visual Studio after setting environment variables

### Qt Libraries Not Found
- Check that Qt's lib directory is accessible
- Verify you're using the correct Qt version (MSVC build)

### Missing DLLs at Runtime
- Copy required Qt DLLs to the output directory, or
- Add Qt's bin directory to your PATH environment variable

### Required Qt DLLs (Release):
- Qt5Core.dll
- Qt5Widgets.dll
- Qt5Network.dll
- Qt5Sql.dll

### Required Qt DLDLs (Debug):
- Qt5Cored.dll
- Qt5Widgetsd.dll
- Qt5Networkd.dll
- Qt5Sqld.dll

## Alternative Build Method

If you prefer CMake, you can still use the existing `CMakeLists.txt`:

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Project Structure in Visual Studio

The solution organizes files into logical folders:
- **Source Files**: Main application entry point
- **UI**: User interface components (MainWindow, dialogs, panels)
- **Core**: Core functionality (HttpClient, SyntaxHighlighter)
- **Models**: Data models
- **Managers**: Various manager classes
- **Testing**: Test-related components
- **Resource Files**: Qt resource files and other assets