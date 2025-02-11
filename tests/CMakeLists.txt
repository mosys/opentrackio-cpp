# Copyright 2025 Mo-Sys Engineering Ltd
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
project(opentrackio-cpp-tests VERSION 0.9.1 LANGUAGES CXX)
Include(FetchContent)

# Catch 2 test framework
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)

# OpenSSL - for Curl
FetchContent_Declare(
    ssl_content
    URL "https://github.com/janbar/openssl-cmake/archive/refs/tags/1.1.1w-20231130.tar.gz"
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(ssl_content)

# Curl - to get the latest examples from OpenTrackIO.org
FetchContent_Declare(
        curl
        URL "https://github.com/curl/curl/archive/refs/tags/curl-8_7_1.tar.gz"
        OVERRIDE_FIND_PACKAGE
)
set(CURL_USE_OPENSSL ON CACHE INTERNAL "Use OpenSSL backend" FORCE)
option(CMAKE_USE_SCHANNEL "Set to ON to use WINSSL for windows." ON)
FetchContent_MakeAvailable(curl)

# Json
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
find_package(nlohmann_json REQUIRED)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${CMAKE_CURRENT_LIST_DIR}/../build/debug/Debug)
else()
    link_directories(${CMAKE_CURRENT_LIST_DIR}/../build/release/Release)
endif()

# Json validator
FetchContent_Declare(nlohmann_json_schema_validator
        GIT_REPOSITORY https://github.com/pboettch/json-schema-validator
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
        GIT_TAG 2.3.0
)
set(nlohmann_json_schema_validator_SHARED_LIBS OFF CACHE BOOL "Build nlohmann_json_schema_validator as a static library" FORCE)
FetchContent_MakeAvailable(nlohmann_json_schema_validator)

# Compile
add_executable(tests
    test.cpp
)
target_compile_features(tests PUBLIC cxx_std_20)
target_include_directories(tests
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../include"
        "${CMAKE_CURRENT_LIST_DIR}/../external"
)

# IDE
target_sources(tests
    PRIVATE
        test.cpp
        ../include/opentrackio-cpp/OpenTrackIOHelper.h
        ../include/opentrackio-cpp/OpenTrackIOProperties.h
        ../include/opentrackio-cpp/OpenTrackIOSample.h
        ../include/opentrackio-cpp/OpenTrackIOTypes.h
        ../src/OpenTrackIOProperties.cpp
        ../src/OpenTrackIOSample.cpp
)

find_package(CURL REQUIRED)

# Linkage
target_link_libraries(tests
    PRIVATE
        Catch2::Catch2WithMain
        ssl
        crypto
        CURL::libcurl
        nlohmann_json_schema_validator
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(tests PRIVATE opentrackio-cppd.lib)
    # Post-build copy debug dlls
    add_custom_command(TARGET tests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_LIST_DIR}/build/_deps/curl-build/lib/Debug/libcurl-d.dll"
            "${CMAKE_CURRENT_LIST_DIR}/build/Debug/"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_LIST_DIR}/build/_deps/catch2-build/src/Debug/Catch2d.dll"
            "${CMAKE_CURRENT_LIST_DIR}/build/Debug/"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_LIST_DIR}/build/_deps/catch2-build/src/Debug/Catch2Maind.dll"
            "${CMAKE_CURRENT_LIST_DIR}/build/Debug/"
    )
else()
    target_link_libraries(tests PRIVATE opentrackio-cpp.lib)
endif()

