# Copyright 2024 Mo-Sys Engineering Ltd
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
# to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.15.7)
project(opentrackio-cpp-tests VERSION 0.9.0 LANGUAGES CXX)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
find_package(nlohmann_json REQUIRED)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${CMAKE_CURRENT_LIST_DIR}/../build/debug/Debug)
else()
    link_directories(${CMAKE_CURRENT_LIST_DIR}/../build/release/Release)
endif()
find_package(opentrackio-cpp REQUIRED PATHS "${CMAKE_CURRENT_LIST_DIR}/../install/cmake")

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(tests PRIVATE opentrackio-cppd.lib)
else()
    target_link_libraries(tests PRIVATE opentrackio-cpp.lib)
endif()
target_include_directories(tests PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../install/include"
    "${CMAKE_CURRENT_LIST_DIR}/../external")
target_compile_features(tests PUBLIC cxx_std_20)
