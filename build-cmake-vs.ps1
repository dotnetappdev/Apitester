#!/usr/bin/env powershell

# ApiTester CMake Build Script for Visual Studio (PowerShell)
Write-Host "ApiTester CMake Build Script for Visual Studio" -ForegroundColor Cyan
Write-Host "=============================================="
Write-Host ""

# Check if CMake is available
try {
    $cmakeVersion = & cmake --version 2>&1
    Write-Host "✓ CMake found" -ForegroundColor Green
} catch {
    Write-Host "✗ CMake is not installed or not in PATH" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install CMake from https://cmake.org/download/"
    Write-Host "and add it to your PATH environment variable."
    Read-Host "Press Enter to exit"
    exit 1
}

# Create build directory
if (-not (Test-Path "build-cmake")) {
    New-Item -ItemType Directory -Path "build-cmake" | Out-Null
    Write-Host "Created build-cmake directory"
}

Set-Location "build-cmake"

Write-Host ""
Write-Host "Configuring project with CMake..."
Write-Host ""

# Configure with Visual Studio generator
try {
    & cmake .. -G "Visual Studio 17 2022" -A x64
    if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed" }
} catch {
    Write-Host ""
    Write-Host "✗ CMake configuration failed!" -ForegroundColor Red
    Write-Host ""
    Write-Host "This is likely due to Qt5 not being found."
    Write-Host "Please ensure Qt5 is installed and either:"
    Write-Host "1. Set QTDIR environment variable, or"
    Write-Host "2. Set CMAKE_PREFIX_PATH to your Qt installation, or"
    Write-Host "3. Run setup-qt.ps1 first"
    Write-Host ""
    Write-Host "Example:"
    Write-Host "`$env:CMAKE_PREFIX_PATH = 'C:\Qt\5.15.2\msvc2019_64'"
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "✓ Project configured successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "Building project..."
Write-Host ""

# Build the project
try {
    & cmake --build . --config Release
    if ($LASTEXITCODE -ne 0) { throw "Build failed" }
} catch {
    Write-Host ""
    Write-Host "✗ Build failed!" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "✓ Build completed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "Executable location: build-cmake\Release\ApiTester.exe" -ForegroundColor Cyan
Write-Host ""
Write-Host "You can also open build-cmake\ApiTester.sln in Visual Studio"
Write-Host "for debugging and development."
Write-Host ""
Read-Host "Press Enter to exit"