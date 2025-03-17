# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.30.8)

project(magicxx
    VERSION 5.6.3
    DESCRIPTION "A C++ wrapper library over the Magic Number Recognition Library."
    HOMEPAGE_URL "https://github.com/oguztoraman/libmagicxx"
    LANGUAGES C CXX
)

option(BUILD_AS_STATIC "Build as static." OFF)
option(BUILD_MAGICXX_TESTS "Build the tests." OFF)
option(BUILD_MAGICXX_EXAMPLES "Build the examples." OFF)
option(BUILD_MAGICXX_DOCUMENTATION_ONLY "Build only the documentation." OFF)

if (BUILD_MAGICXX_DOCUMENTATION_ONLY)
    add_subdirectory(doc)
    return()
endif()

if(BUILD_AS_STATIC)
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
    ${magic_SOURCE_DIR}/buffer.c
    ${magic_SOURCE_DIR}/cdf.c
    ${magic_SOURCE_DIR}/cdf_time.c
    ${magic_SOURCE_DIR}/compress.c
    ${magic_SOURCE_DIR}/der.c
    ${magic_SOURCE_DIR}/encoding.c
    ${magic_SOURCE_DIR}/fmtcheck.c
    ${magic_SOURCE_DIR}/fsmagic.c
    ${magic_SOURCE_DIR}/funcs.c
    ${magic_SOURCE_DIR}/is_csv.c
    ${magic_SOURCE_DIR}/is_json.c
    ${magic_SOURCE_DIR}/is_simh.c
    ${magic_SOURCE_DIR}/is_tar.c
    ${magic_SOURCE_DIR}/magic.c
    ${magic_SOURCE_DIR}/print.c
    ${magic_SOURCE_DIR}/readcdf.c
    ${magic_SOURCE_DIR}/readelf.c
    ${magic_SOURCE_DIR}/softmagic.c
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

include(cmake/configure_libmagic.cmake)

add_library(magicxx ${LIBRARY_TYPE})

set_target_properties(magicxx PROPERTIES
    C_STANDARD 99
    C_EXTENSIONS OFF
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    VERSION ${magicxx_VERSION}
    SOVERSION ${magicxx_VERSION_MAJOR}
)

target_sources(magicxx
    PUBLIC
        FILE_SET magicxx_headers
        TYPE HEADERS
        BASE_DIRS ${magicxx_INCLUDE_DIR}
        FILES ${magicxx_HEADER_FILES}
    PRIVATE
        ${magic_SOURCE_FILES}
        ${magicxx_SOURCE_FILES}
)

target_compile_definitions(magicxx
    PRIVATE
        HAVE_CONFIG_H
        HAVE_STDINT_H
)

target_include_directories(magicxx
    PRIVATE
        ${magic_INCLUDE_DIR}
    PUBLIC
        ${magicxx_INCLUDE_DIR}
)

if (BUILD_MAGICXX_TESTS)
    enable_testing()
    set(INSTALL_GTEST OFF)
    add_subdirectory(googletest)
    add_subdirectory(tests)
endif()

if (BUILD_MAGICXX_EXAMPLES)
    add_subdirectory(examples)
endif()
