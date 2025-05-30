# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

if (BUILD_MAGICXX_SHARED_LIB)
    add_library(magicxx SHARED)

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

    target_include_directories(magicxx
        PRIVATE
            ${gnurx_INCLUDE_DIR}
            ${magic_INCLUDE_DIR}
        PUBLIC
            $<BUILD_INTERFACE:${magicxx_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:${magicxx_INSTALL_INCLUDE_DIR}>
    )
endif(BUILD_MAGICXX_SHARED_LIB)

if (BUILD_MAGICXX_STATIC_LIB)
    add_library(magicxx_static STATIC)

    add_library(recognition::magicxx_static ALIAS magicxx_static)

    add_dependencies(magicxx_static configure_file)

    set_target_properties(magicxx_static PROPERTIES
        C_STANDARD 99
        C_EXTENSIONS OFF
        C_STANDARD_REQUIRED ON
        CXX_STANDARD 23
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
    )

    target_sources(magicxx_static
        PUBLIC
            FILE_SET magicxx_staticHeaders
            TYPE HEADERS
            BASE_DIRS ./include
            FILES ${magicxx_HEADER_FILES}
        PRIVATE
            ${gnurx_SOURCE_FILES}
            ${magic_SOURCE_FILES}
            ${magicxx_SOURCE_FILES}
    )

    target_compile_definitions(magicxx_static
        PRIVATE
            HAVE_CONFIG_H
            HAVE_STDINT_H
            DEFAULT_DATABASE_FILE="${magicxx_INSTALLED_DEFAULT_DATABASE_FILE}"
    )

    target_include_directories(magicxx_static
        PRIVATE
            ${gnurx_INCLUDE_DIR}
            ${magic_INCLUDE_DIR}
        PUBLIC
            $<BUILD_INTERFACE:${magicxx_INCLUDE_DIR}>
            $<INSTALL_INTERFACE:${magicxx_INSTALL_INCLUDE_DIR}>
    )
endif(BUILD_MAGICXX_STATIC_LIB)
