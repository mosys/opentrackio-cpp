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

echo ========================================
echo Installing OpenTrackIO C++ Library (System-wide)
echo ========================================
echo.
echo This will install the library to the default system CMake paths.
echo Administrator privileges may be required.
echo.

REM Clean previous build
if exist build (
    echo Removing previous build directory...
    rd /s /q build
)

echo.
echo Configuring Release build for system installation...
echo Note: By default builds shared library (DLL). To build static library, add -DBUILD_STATIC_LIBS=ON
cmake -B build/release -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo Error: Release configuration failed
    exit /b 1
)

echo.
echo Configuring Debug build for system installation...
cmake -B build/debug -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 (
    echo Error: Debug configuration failed
    exit /b 1
)

echo.
echo Building and installing Debug configuration...
cmake --build build/debug --config Debug --target install
if errorlevel 1 (
    echo Error: Debug build/install failed
    exit /b 1
)

echo.
echo Building and installing Release configuration...
cmake --build build/release --config Release --target install
if errorlevel 1 (
    echo Error: Release build/install failed
    exit /b 1
)

echo.
echo ========================================
echo System-wide installation completed successfully!
echo You can now use find_package(opentrackio-cpp) in your CMake projects.
echo ========================================

endlocal

