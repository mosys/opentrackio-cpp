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

Alternatively one could do a [system-wide](#system-wide) build of the project to install the library to the standard CMake
library paths where `find_package` can be used without any additional setup.

### Build Instructions

#### Requirements:

 - A C++20 compiler (Project has been tested with `MSVC` on Windows and `G++` on Linux).
 - Cmake `>=3.20`

#### Note:

Both `Windows` and `Linux` build scripts write to the same `./install` folder but their Cmake configs and targets
are separated by OS. This means that you can run both scripts to have builds for both systems in the same artefact
if you plan on targeting both `Windows` and `Linux` with your project.

#### Windows:

Run the `build.bat` script in order to build both `Debug` and `Release` version of the library into the 
`./install` folder. The artefact in the `./install` can then be imported into your own project via Cmake's
`find_package` feature.

#### Linux:

Run the `build.sh` script in order to build both `Debug` and `Release` version of the library into the
`./install` folder. The artefact in the `./install` can then be imported into your own project via Cmake's
`find_package` feature.

#### System-wide:

Run the `install.bat/sh` (depending on OS) to install the library to the system itself. This allows you to just use
`find_package` without needing to set `opentrackio-cpp_DIR` in your Cmake or adding the path to the library to your
`CMAKE_PREFIX_PATH`.

## Licence

The MIT License (MIT)

Copyright (c) 2024 Mo-Sys Engineering Ltd

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
