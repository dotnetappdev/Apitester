@echo off
echo ApiTester CMake Build Script for Visual Studio
echo =============================================
echo.

REM Check if CMake is available
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ✗ CMake is not installed or not in PATH
    echo.
    echo Please install CMake from https://cmake.org/download/
    echo and add it to your PATH environment variable.
    pause
    exit /b 1
)

echo ✓ CMake found

REM Create build directory
if not exist "build-cmake" (
    mkdir build-cmake
    echo Created build-cmake directory
)

cd build-cmake

echo.
echo Configuring project with CMake...
echo.

REM Configure with Visual Studio generator
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo.
    echo ✗ CMake configuration failed!
    echo.
    echo This is likely due to Qt5 not being found.
    echo Please ensure Qt5 is installed and either:
    echo 1. Set QTDIR environment variable, or
    echo 2. Set CMAKE_PREFIX_PATH to your Qt installation, or  
    echo 3. Run setup-qt.bat first
    echo.
    echo Example:
    echo set CMAKE_PREFIX_PATH=C:\Qt\5.15.2\msvc2019_64
    echo.
    pause
    exit /b 1
)

echo.
echo ✓ Project configured successfully!
echo.
echo Building project...
echo.

REM Build the project
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo.
    echo ✗ Build failed!
    pause
    exit /b 1
)

echo.
echo ✓ Build completed successfully!
echo.
echo Executable location: build-cmake\Release\ApiTester.exe
echo.
echo You can also open build-cmake\ApiTester.sln in Visual Studio
echo for debugging and development.
echo.
pause