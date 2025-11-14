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

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..

cd /d "%PROJECT_ROOT%"

echo ========================================
echo Building OpenTrackIO C++ Tests
echo ========================================
echo.
echo.
echo Configuring CMake with tests enabled (building as static library)...
cmake -B build -G "Visual Studio 17 2022" -A x64 -DOPENTRACKIO_BUILD_TESTS=ON -DBUILD_STATIC_LIBS=ON
if errorlevel 1 (
    echo Error: CMake configuration failed
    exit /b 1
)

echo.
echo Building tests in Debug mode...
cmake --build build --config Debug --target tests
if errorlevel 1 (
    echo Error: Build failed
    exit /b 1
)

echo.
echo ========================================
echo Running Tests
echo ========================================
echo.

REM Run the tests executable
build\tests\Debug\tests.exe
if errorlevel 1 (
    echo.
    echo Tests FAILED
    exit /b 1
)

echo.
echo ========================================
echo All tests passed successfully!
echo ========================================

endlocal

