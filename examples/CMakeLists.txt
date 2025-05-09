# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.30.8)

project(magicxx_examples LANGUAGES CXX)

find_package(magicxx REQUIRED)

if (NOT MAGICXX_FOUND)
    message(FATAL_ERROR "Libmagicxx not found. Please install it first.")
else()
    message(STATUS "Libmagicxx found.")
    message(STATUS "Libmagicxx version: ${MAGICXX_VERSION}")
    message(STATUS "Libmagicxx include directory: ${MAGICXX_INCLUDE_DIR}")
    message(STATUS "Libmagicxx library directory: ${MAGICXX_LIB_DIR}")
    message(STATUS "Libmagicxx documentation directory: ${MAGICXX_DOC_DIR}")
    message(STATUS "Libmagicxx CMake directory: ${MAGICXX_CMAKE_DIR}")
    message(STATUS "Libmagicxx default database directory: ${MAGICXX_DEFAULT_DATABASE_DIR}")
endif()

set(magicxx_examples_SOURCE_FILES
    magic_examples.cpp
)

add_executable(magicxx_examples)

set_target_properties(magicxx_examples PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

target_sources(magicxx_examples
    PRIVATE
        ${magicxx_examples_SOURCE_FILES}
)

target_link_libraries(magicxx_examples
    PRIVATE
        recognition::magicxx
)

add_executable(magicxx_examples_static)

set_target_properties(magicxx_examples_static PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

target_sources(magicxx_examples_static
    PRIVATE
        ${magicxx_examples_SOURCE_FILES}
)

target_link_libraries(magicxx_examples_static
    PRIVATE
        recognition::magicxx_static
)
