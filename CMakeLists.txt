# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.21.0)

project(magicxx
    VERSION 5.5.1
    DESCRIPTION "A C++ wrapper library over the Magic Number Recognition Library."
    HOMEPAGE_URL "https://github.com/oguztoraman/libmagicxx"
    LANGUAGES C CXX
)

option(BUILD_AS_STATIC "Build as static." OFF)
option(BUILD_MAGICXX_TESTS "Build the tests." OFF)
option(BUILD_MAGICXX_EXAMPLES "Build the examples." OFF)

if (${BUILD_AS_STATIC})
    set(LIBRARY_TYPE STATIC)
else()
    set(LIBRARY_TYPE SHARED)
endif()

set(magic_DIR
    ${magicxx_SOURCE_DIR}/file
)

set(magic_INCLUDE_DIR
    ${magic_DIR}
    ${magic_DIR}/src
)

set(magic_SOURCE_DIR
    ${magic_DIR}/src
)

set(magic_SOURCE_FILES
    ${magic_SOURCE_DIR}/apprentice.c
    ${magic_SOURCE_DIR}/apptype.c
    ${magic_SOURCE_DIR}/ascmagic.c
    ${magic_SOURCE_DIR}/asctime_r.c
    ${magic_SOURCE_DIR}/asprintf.c
    ${magic_SOURCE_DIR}/buffer.c
    ${magic_SOURCE_DIR}/cdf.c
    ${magic_SOURCE_DIR}/cdf_time.c
    ${magic_SOURCE_DIR}/compress.c
    ${magic_SOURCE_DIR}/ctime_r.c
    ${magic_SOURCE_DIR}/der.c
    ${magic_SOURCE_DIR}/dprintf.c
    ${magic_SOURCE_DIR}/encoding.c
    ${magic_SOURCE_DIR}/fmtcheck.c
    ${magic_SOURCE_DIR}/fsmagic.c
    ${magic_SOURCE_DIR}/funcs.c
    ${magic_SOURCE_DIR}/getline.c
    ${magic_SOURCE_DIR}/getopt_long.c
    ${magic_SOURCE_DIR}/gmtime_r.c
    ${magic_SOURCE_DIR}/is_csv.c
    ${magic_SOURCE_DIR}/is_json.c
    ${magic_SOURCE_DIR}/is_simh.c
    ${magic_SOURCE_DIR}/is_tar.c
    ${magic_SOURCE_DIR}/localtime_r.c
    ${magic_SOURCE_DIR}/magic.c
    ${magic_SOURCE_DIR}/pread.c
    ${magic_SOURCE_DIR}/print.c
    ${magic_SOURCE_DIR}/readcdf.c
    ${magic_SOURCE_DIR}/readelf.c
    ${magic_SOURCE_DIR}/seccomp.c
    ${magic_SOURCE_DIR}/softmagic.c
    ${magic_SOURCE_DIR}/strcasestr.c
    ${magic_SOURCE_DIR}/strlcat.c
    ${magic_SOURCE_DIR}/strlcpy.c
    ${magic_SOURCE_DIR}/vasprintf.c
)

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
    ${magicxx_SOURCE_DIR}/tests
)

set(googletest_DIR
    ${magicxx_SOURCE_DIR}/googletest
)

message(STATUS "Initializing and updating git submodules...")
execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${magicxx_SOURCE_DIR}
    RESULT_VARIABLE submodule_result
)
if (submodule_result)
    message(FATAL_ERROR "Failed to update git submodules.")
endif()

message(STATUS "Configuring the file...")
execute_process(
    COMMAND autoreconf -f -i
    WORKING_DIRECTORY ${magic_DIR}
    RESULT_VARIABLE autoreconf_result
)
if (autoreconf_result)
    message(FATAL_ERROR "Failed to run autoreconf.")
endif()

execute_process(
    COMMAND ./configure --enable-static=yes --enable-shared=yes
        --disable-zlib --disable-bzlib
        --disable-xzlib --disable-zstdlib
        --disable-lzlib
    WORKING_DIRECTORY ${magic_DIR}
    RESULT_VARIABLE configure_result
)
if (configure_result)
    message(FATAL_ERROR "Failed to configure file.")
endif()

execute_process(
    COMMAND make magic.h
    WORKING_DIRECTORY ${magic_SOURCE_DIR}
    RESULT_VARIABLE make_result
)
if (make_result)
    message(FATAL_ERROR "Failed to make.")
endif()

add_library(magicxx ${LIBRARY_TYPE}
    ${magic_SOURCE_FILES}
    ${magicxx_HEADER_FILES}
    ${magicxx_SOURCE_FILES}
)

set_target_properties(magicxx PROPERTIES
    C_STANDARD 99
    C_EXTENSIONS OFF
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    VERSION ${magicxx_VERSION}
    SOVERSION ${magicxx_VERSION_MAJOR}
    COMPILE_DEFINITIONS "HAVE_STDINT_H;HAVE_CONFIG_H"
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

if (BUILD_MAGICXX_EXAMPLES)
    add_subdirectory(examples)
endif()
