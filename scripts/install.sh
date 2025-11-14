#!/bin/bash

# Copyright 2025 Mo-Sys Engineering Ltd
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

set -e  # Exit on error

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."

cd "$PROJECT_ROOT"

echo "========================================"
echo "Installing OpenTrackIO C++ Library (System-wide)"
echo "========================================"
echo
echo "This will install the library to the default system CMake paths."
echo "Administrator privileges may be required (use sudo if needed)."
echo

# Clean previous build
if [ -d "build" ]; then
    echo "Removing previous build directory..."
    rm -rf build
fi

echo
echo "Configuring Release build for system installation..."
echo "Note: By default builds shared library. To build static library, add -DBUILD_STATIC_LIBS=ON"
cmake -B build/release -DCMAKE_BUILD_TYPE=Release

echo
echo "Building and installing..."
cmake --build build/release --target install

echo
echo "========================================"
echo "System-wide installation completed successfully!"
echo "You can now use find_package(opentrackio-cpp) in your CMake projects."
echo "========================================"

