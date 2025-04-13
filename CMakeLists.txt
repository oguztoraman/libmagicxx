# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.30.8)

project(magicxx
    VERSION 5.7.1
    DESCRIPTION "A C++ wrapper library over the Magic Number Recognition Library."
    HOMEPAGE_URL "https://github.com/oguztoraman/libmagicxx"
    LANGUAGES C CXX
)

option(BUILD_MAGICXX_AS_STATIC "Build as static." OFF)
option(BUILD_MAGICXX_TESTS "Build the tests." OFF)
option(BUILD_MAGICXX_EXAMPLES "Build the examples." OFF)
option(BUILD_MAGICXX_DOCUMENTATION "Build the documentation." OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (BUILD_MAGICXX_AS_STATIC)
    set(LIBRARY_TYPE STATIC)
else()
    set(LIBRARY_TYPE SHARED)
endif()

set(magic_DIR
    ${magicxx_SOURCE_DIR}/external/file
)

set(magic_INCLUDE_DIR
    ${magic_DIR}
    ${magic_DIR}/src
)

set(magic_SOURCE_DIR
    ${magic_DIR}/src
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(gnurx_DIR
        ${magicxx_SOURCE_DIR}/external/libgnurx
    )

    set(gnurx_INCLUDE_DIR
        ${gnurx_DIR}
    )

    set(gnurx_SOURCE_FILES
        ${gnurx_DIR}/regex.c
    )

    set(magic_OS_REPLACEMENT_SOURCE_FILES
        ${magic_SOURCE_DIR}/asctime_r.c
        ${magic_SOURCE_DIR}/ctime_r.c
        ${magic_SOURCE_DIR}/getline.c
        ${magic_SOURCE_DIR}/gmtime_r.c
        ${magic_SOURCE_DIR}/localtime_r.c
        ${magic_SOURCE_DIR}/pread.c
        ${magic_SOURCE_DIR}/strcasestr.c
        ${magic_SOURCE_DIR}/strlcpy.c
    )

    set(magicxx_OS_DEFAULT_DATABASE_FILE
        "C:\\Program Files\\libmagicxx\\database\\magic"
    )
else()
    set(magicxx_OS_DEFAULT_DATABASE_FILE
        "/usr/share/libmagicxx/database/magic"
    )
endif()

set(magic_HEADER_FILES
    ${magic_SOURCE_DIR}/magic.h
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
    ${magic_OS_REPLACEMENT_SOURCE_FILES}
)

set(magicxx_DEFAULT_DATABASE_PATH
    ${magicxx_SOURCE_DIR}/database
)

set(magicxx_DEFAULT_DATABASE_FILE
    ${magicxx_DEFAULT_DATABASE_PATH}/magic
)

set(magicxx_DEFAULT_COMPILED_DATABASE_FILE
    ${magicxx_DEFAULT_DATABASE_PATH}/magic.mgc
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

include(cmake/dependencies.cmake)

add_library(magicxx ${LIBRARY_TYPE})

add_dependencies(magicxx configure_file)

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
        ${gnurx_SOURCE_FILES}
        ${magic_SOURCE_FILES}
        ${magicxx_SOURCE_FILES}
)

target_compile_definitions(magicxx
    PRIVATE
        HAVE_CONFIG_H
        HAVE_STDINT_H
        DEFAULT_DATABASE_FILE="${magicxx_OS_DEFAULT_DATABASE_FILE}"
)

target_include_directories(magicxx
    PRIVATE
        ${gnurx_INCLUDE_DIR}
        ${magic_INCLUDE_DIR}
    PUBLIC
        ${magicxx_INCLUDE_DIR}
)

if (BUILD_MAGICXX_TESTS)
    enable_testing()
    set(INSTALL_GTEST OFF)
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()

if (BUILD_MAGICXX_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_MAGICXX_DOCUMENTATION)
    add_subdirectory(doc)
endif()

include(cmake/format_source_code.cmake)
