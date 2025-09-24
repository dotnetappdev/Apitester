#!/usr/bin/env powershell

# ApiTester Qt5 Setup Helper (PowerShell)
Write-Host "ApiTester Qt5 Setup Helper" -ForegroundColor Cyan
Write-Host "===========================`n"

# Check if QTDIR is already set
$qtdir = $env:QTDIR
if ($qtdir) {
    Write-Host "QTDIR environment variable is set to: $qtdir"
    if (Test-Path $qtdir) {
        Write-Host "✓ Qt installation found at: $qtdir" -ForegroundColor Green
        $foundQt = $qtdir
    } else {
        Write-Host "✗ Path does not exist: $qtdir" -ForegroundColor Red
        Write-Host ""
        $qtdir = $null
    }
} else {
    Write-Host "QTDIR environment variable is not set."
    Write-Host ""
}

if (-not $foundQt) {
    Write-Host "Searching for Qt5 installations..."
    Write-Host ""

    # Check common Qt installation paths
    $qtPaths = @(
        "C:\Qt\5.15.2\msvc2019_64",
        "C:\Qt\5.15.1\msvc2019_64", 
        "C:\Qt\5.14.2\msvc2017_64",
        "C:\Qt\6.0.0\msvc2019_64"
    )

    foreach ($path in $qtPaths) {
        if (Test-Path $path) {
            Write-Host "✓ Found Qt installation: $path" -ForegroundColor Green
            $foundQt = $path
            break
        } else {
            Write-Host "  Checked: $path (not found)" -ForegroundColor Gray
        }
    }

    # Check vcpkg
    if ($env:VCPKG_ROOT) {
        $vcpkgQtPath = "$env:VCPKG_ROOT\installed\x64-windows"
        if (Test-Path $vcpkgQtPath) {
            Write-Host "✓ Found vcpkg Qt installation: $vcpkgQtPath" -ForegroundColor Green
            $foundQt = $vcpkgQtPath
        }
    }
}

if (-not $foundQt) {
    Write-Host ""
    Write-Host "✗ No Qt5 installation found!" -ForegroundColor Red
    Write-Host ""
    Write-Host "To install Qt5:"
    Write-Host "1. Download Qt installer from https://www.qt.io/download-qt-installer"
    Write-Host "2. Install Qt 5.15.x with MSVC 2019 64-bit compiler"
    Write-Host "3. Or use vcpkg: vcpkg install qt5:x64-windows"
    Write-Host ""
    Write-Host "After installation, run this script again."
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "Setting QTDIR to: $foundQt"

# Set environment variable for current session
$env:QTDIR = $foundQt

# Set environment variable permanently
try {
    [System.Environment]::SetEnvironmentVariable("QTDIR", $foundQt, [System.EnvironmentVariableTarget]::User)
    Write-Host "✓ QTDIR environment variable set permanently" -ForegroundColor Green
} catch {
    Write-Host "⚠ Could not set QTDIR permanently: $($_.Exception.Message)" -ForegroundColor Yellow
    Write-Host "  Set manually: Control Panel → System → Environment Variables"
}

# Validate Qt installation
Write-Host ""
Write-Host "Validating Qt installation..."

$qtInclude = Join-Path $foundQt "include"
$qtLib = Join-Path $foundQt "lib"

if (-not (Test-Path (Join-Path $qtInclude "QtCore"))) {
    Write-Host "✗ QtCore headers not found in $qtInclude\QtCore" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

if (-not (Test-Path (Join-Path $qtInclude "QtWidgets"))) {
    Write-Host "✗ QtWidgets headers not found in $qtInclude\QtWidgets" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

if (-not (Test-Path (Join-Path $qtLib "Qt5Core.lib"))) {
    Write-Host "✗ Qt5Core.lib not found in $qtLib" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "✓ All required Qt5 components found!" -ForegroundColor Green
Write-Host ""
Write-Host "You can now build ApiTester in Visual Studio." -ForegroundColor Cyan
Write-Host ""
Write-Host "Build steps:"
Write-Host "1. Open ApiTester.sln in Visual Studio"
Write-Host "2. Select Debug or Release configuration (x64)"
Write-Host "3. Build Solution (Ctrl+Shift+B)"
Write-Host ""
Read-Host "Press Enter to exit"