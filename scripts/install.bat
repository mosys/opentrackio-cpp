@echo off

@REM Copyright 2025 Mo-Sys Engineering Ltd
@REM Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
@REM to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish,
@REM distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
@REM subject to the following conditions:
@REM
@REM The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
@REM THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@REM FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
@REM DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

setlocal

REM Get the directory where this script is located
set SCRIPT_DIR=%~dp0
REM Get the project root (one level up from scripts/)
set PROJECT_ROOT=%SCRIPT_DIR%..

REM Change to project root
cd /d "%PROJECT_ROOT%"

REM Get absolute path for install directory
for %%i in ("%PROJECT_ROOT%") do set "PROJECT_ROOT=%%~fi"
set "INSTALL_DIR=%PROJECT_ROOT%\install"

echo ========================================
echo Installing OpenTrackIO C++ Library (Local)
echo ========================================
echo.
echo This will install the library to: %INSTALL_DIR%
echo.

REM Clean previous build
if exist build (
    echo Removing previous build directory...
    rd /s /q build
)

REM Clean previous install
if exist install (
    echo Removing previous install directory...
    rd /s /q install
)

echo.
echo Configuring Release build for local installation...
echo Note: By default builds shared library (DLL). To build static library, add -DBUILD_STATIC_LIBS=ON
cmake -B build/release -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"
if errorlevel 1 (
    echo Error: Release configuration failed
    exit /b 1
)

echo.
echo Building and installing Release configuration...
cmake --build build/release --config Release
if errorlevel 1 (
    echo Error: Release build failed
    exit /b 1
)

cmake --install build/release --config Release
if errorlevel 1 (
    echo Error: Release install failed
    exit /b 1
)

echo.
echo ========================================
echo Local installation completed successfully!
echo Installed to: %INSTALL_DIR%
echo To use in CMake projects, set: -DCMAKE_PREFIX_PATH=%INSTALL_DIR%
echo ========================================

endlocal
