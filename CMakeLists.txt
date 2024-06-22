# SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.21.0)

project(magicxx
    VERSION 5.0.0
    DESCRIPTION "A C++ wrapper library over the Magic Number Recognition Library."
    HOMEPAGE_URL "https://github.com/oguztoraman/libmagicxx"
    LANGUAGES CXX
)

option(BUILD_MAGICXX_TESTS "Build the tests." OFF)

set(magic_INCLUDE_DIR
    ${magicxx_SOURCE_DIR}/file/src
)

set(magic_LIBRARY
    ${magic_INCLUDE_DIR}/.libs/libmagic.so
)

if (NOT EXISTS ${magic_LIBRARY})
    message(STATUS "Running initialization and setup steps...")

    message(STATUS "Initializing and updating git submodules...")
    execute_process(
        COMMAND git submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE submodule_result
    )
    if (submodule_result)
        message(FATAL_ERROR "Failed to update git submodules.")
    endif()

    message(STATUS "Configuring the file...")
    execute_process(
        COMMAND autoreconf -f -i
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/file
        RESULT_VARIABLE autoreconf_result
    )
    if (autoreconf_result)
        message(FATAL_ERROR "Failed to run autoreconf.")
    endif()

    execute_process(
        COMMAND ./configure
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/file
        RESULT_VARIABLE configure_result
    )
    if (configure_result)
        message(FATAL_ERROR "Failed to configure file.")
    endif()

    message(STATUS "Building the file command and the Magic Number Recognition Library...")
    execute_process(
        COMMAND make
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/file
        RESULT_VARIABLE make_result
    )
    if (make_result)
        message(FATAL_ERROR "Failed to build the file command and the Magic Number Recognition Library.")
    endif()

    message(STATUS "Done")
endif()

set(magicxx_INCLUDE_DIR
    ${magicxx_SOURCE_DIR}/inc
)

set(magicxx_HEADER_FILES
    ${magicxx_INCLUDE_DIR}/file_concepts.hpp
    ${magicxx_INCLUDE_DIR}/magic.hpp
    ${magicxx_INCLUDE_DIR}/magic_exception.hpp
    ${magicxx_INCLUDE_DIR}/utility.hpp
)

set(magicxx_SOURCE_FILES
    ${magicxx_SOURCE_DIR}/src/magic.cpp
)

set(magicxx_TEST_DIR
    ${magicxx_SOURCE_DIR}/test
)

set(googletest_DIR
    ${magicxx_SOURCE_DIR}/googletest
)

add_library(magicxx SHARED)

set_target_properties(magicxx PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    SOURCES ${magicxx_SOURCE_FILES}
    VERSION ${magicxx_VERSION}
    SOVERSION ${magicxx_VERSION_MAJOR}
    LINK_LIBRARIES ${magic_LIBRARY}
    COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wfatal-errors;$<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>>"
)

target_include_directories(magicxx
    PRIVATE ${magic_INCLUDE_DIR}
    PUBLIC ${magicxx_INCLUDE_DIR}
)

if (BUILD_MAGICXX_TESTS)
    set(INSTALL_GTEST OFF)
    add_compile_options("$<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>")
    add_subdirectory(${googletest_DIR})
    add_subdirectory(${magicxx_TEST_DIR})
endif()
