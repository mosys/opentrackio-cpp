@echo off

@REM Copyright 2025 Mo-Sys Engineering Ltd
@REM Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), 
@REM to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, 
@REM distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
@REM subject to the following conditions:
@REM 
@REM The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
@REM THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
@REM FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
@REM DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

rd /s /q %~dp0\build

cmake -B build/debug -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./install
cmake -B build/release -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./install

cmake --build build/debug --config Debug --target install
cmake --build build/release --config Release --target install
