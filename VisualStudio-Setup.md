# Visual Studio Setup Guide for ApiTester

This guide helps you build the ApiTester application using Visual Studio on Windows.

## Quick Setup

For the fastest setup, run one of the automated setup scripts:

**Windows Command Prompt:**
```cmd
setup-qt.bat
```

**PowerShell:**
```powershell
.\setup-qt.ps1
```

These scripts will:
- Check if Qt5 is already installed and configured
- Search for common Qt5 installation paths
- Set the QTDIR environment variable automatically
- Validate the Qt5 installation

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

## Environment Setup (Automatic)

**Recommended**: Run `setup-qt.bat` from the project root directory. This script will automatically detect and configure Qt5 for you.

## Environment Setup (Manual)

If you prefer to set up manually or the automatic setup fails:

1. Set the `QTDIR` environment variable to your Qt installation path:
   - Example: `C:\Qt\5.15.2\msvc2019_64`
   - Add this to your system environment variables

2. Add Qt's bin directory to your PATH:
   - Example: `C:\Qt\5.15.2\msvc2019_64\bin`

## Building in Visual Studio

1. Open `ApiTester.sln` in Visual Studio
2. Select your desired configuration (Debug/Release) and platform (x64)
3. Build the solution (Ctrl+Shift+B)

**Note**: The project now includes automatic Qt detection. If Qt5 is not found, you'll get a clear error message with installation instructions.

The executable will be created in:
- Debug: `build\Debug\ApiTester.exe`
- Release: `build\Release\ApiTester.exe`

## Configuration Notes

- The project is configured for x64 builds only
- Qt libraries are linked dynamically
- The project automatically detects Qt5 installations in common locations
- If QTDIR is not set, the project will attempt to find Qt5 automatically
- Make sure Qt DLLs are accessible when running the application

## Troubleshooting

### Qt Headers Not Found (Updated)

**First**, try the automated setup:
```cmd
setup-qt.bat
```
or
```powershell
.\setup-qt.ps1
```

**If that fails**:
- Verify Qt5 is installed with MSVC 2019 64-bit compiler
- Set `QTDIR` environment variable manually to your Qt installation path
- Restart Visual Studio after setting environment variables

**Common Qt installation paths**:
- `C:\Qt\5.15.2\msvc2019_64`
- `C:\Qt\5.15.1\msvc2019_64`
- `C:\Qt\5.14.2\msvc2017_64`

### Build Errors with Qt

The project now provides detailed error messages when Qt5 is not found. Look for:
- Clear installation instructions in the build output
- Paths that were checked for Qt5
- Specific missing components

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

## Alternative Build Method (CMake)

If you prefer CMake or encounter issues with the direct Visual Studio build, you can use CMake to generate a Visual Studio solution:

**Automated CMake build:**
```cmd
build-cmake-vs.bat
```
or
```powershell
.\build-cmake-vs.ps1
```

**Manual CMake build:**
```cmd
mkdir build-cmake
cd build-cmake
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

This will create a `build-cmake\ApiTester.sln` file that you can open in Visual Studio for debugging and development.

**Note**: CMake build requires Qt5 to be properly configured via QTDIR or CMAKE_PREFIX_PATH environment variables.

## Project Structure in Visual Studio

The solution organizes files into logical folders:
- **Source Files**: Main application entry point
- **UI**: User interface components (MainWindow, dialogs, panels)
- **Core**: Core functionality (HttpClient, SyntaxHighlighter)
- **Models**: Data models
- **Managers**: Various manager classes
- **Testing**: Test-related components
- **Resource Files**: Qt resource files and other assets