# opentrackio-cpp

`opentrackio-cpp` is a small library with the intention to help handle parsing the JSON
payloads sent via the [SMPTE OpenTrackIO protocol](https://www.opentrackio.org/).

This library makes extensive use of the well established C++ [nlohmann JSON](https://github.com/nlohmann/json) library
in order to take the `Raw Text` or `CBOR` representation of an OpenTrackIO payload and convert it into something more
structured for use within your C++ code.

### Usage:

#### Conan:

If your project uses Conan2 you can fetch this as a dependency from [Conan Center](https://conan.io/center/recipes/opentrackio-cpp).

Follow the instructions on the page to add this as a dependency to your project.

#### CMake:

If your project is already using Cmake you can download a build for the desired platform from the 
[Releases Page](https://github.com/mosys/opentrackio-cpp/releases) import the library into your project via `find_package`.
Simply then add the `{buildDir}/lib/{platform}/cmake` path to either your `CMAKE_PREFIX_PATH` or in your `CMakeLists.txt`
set `opentrackio-cpp_DIR` to that same path.

Alternatively one could do a [system-wide installation](#system-wide-installation) to install the library to the standard CMake
library paths where `find_package` can be used without any additional setup.

### Build Instructions

#### Requirements:

 - A C++20 compiler (Project has been tested with `MSVC` on Windows and `G++` on Linux).
 - Cmake `>=3.20`

#### Note:

Both `Windows` and `Linux` build scripts write to the same `./install` folder but their Cmake configs and targets
are separated by OS. This means that you can run both scripts to have builds for both systems in the same artefact
if you plan on targeting both `Windows` and `Linux` with your project.

#### Building for Local Use

Run the appropriate build script from the `scripts/` directory to build both `Debug` and `Release` versions of the 
library. The artefact can then be imported into your own project via CMake's `find_package` feature.

**By default, the library builds as a shared library.**

**Windows:**
```batch
scripts\build.bat
```

**Linux/macOS:**
```bash
./scripts/build.sh
```

**To build as a static library instead:**

**Windows:**
```batch
cmake -B build/release -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC_LIBS=ON
cmake --build build/release --config Release
```

**Linux/macOS:**
```bash
cmake -B build/release -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC_LIBS=ON
cmake --build build/release
```

#### System-wide Installation

Run the appropriate install script from the `scripts/` directory to install the library to the system itself. 
This allows you to just use `find_package` without needing to set `opentrackio-cpp_DIR` in your CMake or adding 
the path to the library to your `CMAKE_PREFIX_PATH`.

**Windows:**
```batch
scripts\install.bat
```

**Linux/macOS:**
```bash
./scripts/install.sh
# Note: May require sudo for system-wide installation
```

**To install as a static library:**

**Windows:**
```batch
cmake -B build/release -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC_LIBS=ON
cmake --build build/release --config Release --target install
```

**Linux/macOS:**
```bash
cmake -B build/release -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC_LIBS=ON
cmake --build build/release --target install
# Note: May require sudo for system-wide installation
```

## Tests

The test suite is optional and disabled by default.

### Quick Test with Scripts

The easiest way to build and run tests is using the provided test scripts. These scripts automatically build the library as a static library (required for proper test linking) and run the test suite:

#### Windows:

```batch
scripts\test.bat
```

#### Linux/macOS:

```bash
./scripts/test.sh
```

### Building with Tests Manually

To build the library with tests enabled manually, add the `-DOPENTRACKIO_BUILD_TESTS=ON` flag. **Note: Tests require building as a static library (`-DBUILD_STATIC_LIBS=ON`) for proper linking.**

#### Windows:

```batch
cmake -B build -G "Visual Studio 17 2022" -A x64 -DOPENTRACKIO_BUILD_TESTS=ON -DBUILD_STATIC_LIBS=ON
cmake --build build --config Debug
```

#### Linux/macOS:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DOPENTRACKIO_BUILD_TESTS=ON -DBUILD_STATIC_LIBS=ON
cmake --build build
```

### Running Tests

After building with tests enabled, you can run the tests:

#### Windows:

```batch
# Using the test executable directly
.\build\tests\Debug\tests.exe

# Or using CTest
cd build
ctest -C Debug
```

#### Linux/macOS:

```bash
# Using the test executable directly
./build/tests/tests

# Or using CTest
cd build
ctest
```

## Licence

The MIT License (MIT)

Copyright (c) 2025 Mo-Sys Engineering Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

***

The library uses the [JSON library](https://github.com/nlohmann/json) from [Niels Lohmann](https://nlohmann.me) which is licensed under the 
[MIT License](https://opensource.org/licenses/MIT) (see above). Copyright 2013-2022
