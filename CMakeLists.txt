# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.30.8)

project(magicxx
    VERSION 6.0.2
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

    set(magicxx_DEFAULT_DATABASES_INSTALL_DIR
        "C:\\Program Files\\magicxx\\databases"
    )
else()
    set(magicxx_DEFAULT_DATABASES_INSTALL_DIR
        "/usr/share/magicxx/databases"
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
    ${magicxx_SOURCE_DIR}/databases
)

set(magicxx_DEFAULT_DATABASE_FILE
    ${magicxx_DEFAULT_DATABASE_PATH}/magic
)

set(magicxx_DEFAULT_COMPILED_DATABASE_FILE
    ${magicxx_DEFAULT_DATABASE_PATH}/magic.mgc
)

set(magicxx_DEFAULT_DATABASE_FILES
    ${magicxx_DEFAULT_DATABASE_FILE}
    ${magicxx_DEFAULT_COMPILED_DATABASE_FILE}
)

set(magicxx_INSTALLED_DEFAULT_DATABASE_FILE
    ${magicxx_DEFAULT_DATABASES_INSTALL_DIR}/magic
)

set(magicxx_INCLUDE_DIR
    ${magicxx_SOURCE_DIR}/include/magicxx
)

set(magicxx_HEADER_FILES
    ${magicxx_INCLUDE_DIR}/file_concepts.hpp
    ${magicxx_INCLUDE_DIR}/magic.hpp
    ${magicxx_INCLUDE_DIR}/magic_exception.hpp
    ${magicxx_INCLUDE_DIR}/utility.hpp
)

set(magicxx_SOURCE_FILES
    ${magicxx_SOURCE_DIR}/sources/magic.cpp
)

include(cmake/dependencies.cmake)

include(cmake/format_source_code.cmake)

include(cmake/generate_default_database_files.cmake)

add_library(magicxx ${LIBRARY_TYPE})

add_library(recognition::magicxx ALIAS magicxx)

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
        FILE_SET magicxxHeaders
        TYPE HEADERS
        BASE_DIRS ./include
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
        DEFAULT_DATABASE_FILE="${magicxx_INSTALLED_DEFAULT_DATABASE_FILE}"
)

include(GNUInstallDirs)

set(magicxx_INSTALL_INCLUDEDIR
    ${CMAKE_INSTALL_INCLUDEDIR}/magicxx
)

target_include_directories(magicxx
    PRIVATE
        ${gnurx_INCLUDE_DIR}
        ${magic_INCLUDE_DIR}
    PUBLIC
        "$<BUILD_INTERFACE:${magicxx_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${magicxx_INSTALL_INCLUDEDIR}>"
)

include(CMakePackageConfigHelpers)

set(magicxx_INSTALL_LIBDIR
    ${CMAKE_INSTALL_LIBDIR}/magicxx
)

set(magicxx_INSTALL_DOCDIR
    ${CMAKE_INSTALL_DOCDIR}
)

set(magicxx_CMAKE_INSTALL_LIBDIR
    ${CMAKE_INSTALL_LIBDIR}/cmake/magicxx
)

set(magicxx_CMAKE_PACKAGE_CONFIG_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/magicxxConfig.cmake
)

set(magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE
    ${CMAKE_CURRENT_BINARY_DIR}/magicxxConfigVersion.cmake
)

configure_package_config_file(
    cmake/magicxxConfig.cmake.in
        ${magicxx_CMAKE_PACKAGE_CONFIG_FILE}
    INSTALL_DESTINATION
        ${magicxx_CMAKE_INSTALL_LIBDIR}
    PATH_VARS
        magicxx_INSTALL_INCLUDEDIR
        magicxx_INSTALL_LIBDIR
        magicxx_INSTALL_DOCDIR
        magicxx_CMAKE_INSTALL_LIBDIR
        magicxx_DEFAULT_DATABASES_INSTALL_DIR
)

write_basic_package_version_file(
    ${magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE}
    VERSION
        ${magicxx_VERSION}
    COMPATIBILITY
        SameMajorVersion
)

install(
    FILES
        ${magicxx_CMAKE_PACKAGE_CONFIG_FILE}
        ${magicxx_CMAKE_PACKAGE_CONFIG_VERSION_FILE}
    DESTINATION
        ${magicxx_CMAKE_INSTALL_LIBDIR}
)

install(
    TARGETS
        magicxx
    EXPORT
        magicxxTargets
    FILE_SET
        magicxxHeaders
    LIBRARY DESTINATION
        ${magicxx_INSTALL_LIBDIR}
    ARCHIVE DESTINATION
        ${magicxx_INSTALL_LIBDIR}
)

install(
    EXPORT
        magicxxTargets
    NAMESPACE
        recognition::
    DESTINATION
        ${magicxx_CMAKE_INSTALL_LIBDIR}
)

install(
    FILES
        ${magicxx_DEFAULT_DATABASE_FILES}
    DESTINATION
        ${magicxx_DEFAULT_DATABASES_INSTALL_DIR}
)

install(
    DIRECTORY
        documentation/html
    DESTINATION
        ${CMAKE_INSTALL_DOCDIR}
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall_magicxx.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE
        @ONLY
)

add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
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
    add_subdirectory(documentation)
endif()
