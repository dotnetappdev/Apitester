@echo off
echo ApiTester Qt5 Setup Helper
echo ===========================
echo.

REM Check if QTDIR is already set
if defined QTDIR (
    echo QTDIR environment variable is set to: %QTDIR%
    if exist "%QTDIR%" (
        echo ✓ Qt installation found at: %QTDIR%
        goto :validate_qt
    ) else (
        echo ✗ Path does not exist: %QTDIR%
        echo.
    )
) else (
    echo QTDIR environment variable is not set.
    echo.
)

echo Searching for Qt5 installations...
echo.

REM Check common Qt installation paths
set QT_PATHS=C:\Qt\5.15.2\msvc2019_64 C:\Qt\5.15.1\msvc2019_64 C:\Qt\5.14.2\msvc2017_64 C:\Qt\6.0.0\msvc2019_64

for %%p in (%QT_PATHS%) do (
    if exist "%%p" (
        echo ✓ Found Qt installation: %%p
        set FOUND_QT=%%p
        goto :found_qt
    ) else (
        echo   Checked: %%p (not found)
    )
)

REM Check vcpkg
if defined VCPKG_ROOT (
    set VCPKG_QT_PATH=%VCPKG_ROOT%\installed\x64-windows
    if exist "%VCPKG_QT_PATH%" (
        echo ✓ Found vcpkg Qt installation: %VCPKG_QT_PATH%
        set FOUND_QT=%VCPKG_QT_PATH%
        goto :found_qt
    )
)

echo.
echo ✗ No Qt5 installation found!
echo.
echo To install Qt5:
echo 1. Download Qt installer from https://www.qt.io/download-qt-installer
echo 2. Install Qt 5.15.x with MSVC 2019 64-bit compiler
echo 3. Or use vcpkg: vcpkg install qt5:x64-windows
echo.
echo After installation, run this script again.
pause
exit /b 1

:found_qt
echo.
echo Setting QTDIR to: %FOUND_QT%

REM Set environment variable for current session
set QTDIR=%FOUND_QT%

REM Set environment variable permanently (requires admin rights)
setx QTDIR "%FOUND_QT%" >nul 2>&1
if %errorlevel%==0 (
    echo ✓ QTDIR environment variable set permanently
) else (
    echo ⚠ Could not set QTDIR permanently (requires admin rights)
    echo   Set manually: Control Panel → System → Environment Variables
)

:validate_qt
echo.
echo Validating Qt installation...

if not exist "%QTDIR%\include\QtCore" (
    echo ✗ QtCore headers not found in %QTDIR%\include\QtCore
    goto :error
)

if not exist "%QTDIR%\include\QtWidgets" (
    echo ✗ QtWidgets headers not found in %QTDIR%\include\QtWidgets
    goto :error
)

if not exist "%QTDIR%\lib\Qt5Core.lib" (
    echo ✗ Qt5Core.lib not found in %QTDIR%\lib
    goto :error
)

echo ✓ All required Qt5 components found!
echo.
echo You can now build ApiTester in Visual Studio.
echo.
echo Build steps:
echo 1. Open ApiTester.sln in Visual Studio
echo 2. Select Debug or Release configuration (x64)
echo 3. Build Solution (Ctrl+Shift+B)
echo.
pause
exit /b 0

:error
echo.
echo ✗ Qt installation appears to be incomplete or corrupted.
echo Please reinstall Qt5 with all required components.
echo.
pause
exit /b 1