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
project(opentrackio-cpp VERSION 1.0.0 LANGUAGES CXX)

set (
        source_list
        
        src/OpenTrackIOProperties.cpp
        src/OpenTrackIOSample.cpp
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(nlohmann_json REQUIRED)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${source_list})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Handle debug vs release builds and their separate libs

# Standard generator
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}d")
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
endif()

# Multi-config generator
if(CMAKE_CONFIGURATION_TYPES)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME_DEBUG "${PROJECT_NAME}d"
            OUTPUT_NAME_RELEASE "${PROJECT_NAME}"
            OUTPUT_NAME_RELWITHDEBINFO "${PROJECT_NAME}"
            OUTPUT_NAME_MINSIZEREL "${PROJECT_NAME}"
    )
endif()

include(GNUInstallDirs)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC 
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            
)

target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Write targets and config out to different folders depending on OS relating to the differing search procedure cmake does.
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(cmake_targets_install_dir ${CMAKE_INSTALL_PREFIX}/cmake)
else()
    set(cmake_targets_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif()

install(EXPORT ${PROJECT_NAME}Targets
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${cmake_targets_install_dir}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/${PROJECT_NAME}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# If json dep isn't covered by conan then copy ours to the install
get_target_property(IS_CONAN_PACKAGE nlohmann_json::nlohmann_json INTERFACE_COMPILE_OPTIONS)
if (NOT IS_CONAN_PACKAGE)
    message("Build isn't Conan. Copying JSON deps.")
    install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/external/nlohmann/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/external/nlohmann/)
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION ${cmake_targets_install_dir}
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION ${cmake_targets_install_dir}
)
