# SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.21.0)

project(magicxx
    VERSION 4.1.1
    DESCRIPTION "A C++ wrapper library over the Magic Number Recognition Library."
    HOMEPAGE_URL "https://github.com/oguztoraman/libmagicxx"
    LANGUAGES CXX
)

set(magic_INCLUDE_DIR
    ${magicxx_SOURCE_DIR}/file/src
)

set(magic_LIBRARY
    ${magic_INCLUDE_DIR}/.libs/libmagic.so
)

if (NOT EXISTS ${magic_LIBRARY})
    message(FATAL_ERROR "First, you need to run the setup.sh bash script.")
endif()

set(magicxx_INCLUDE_DIR
    ${magicxx_SOURCE_DIR}/inc
)

set(magicxx_HEADER_FILES
    ${magicxx_INCLUDE_DIR}/file_concepts.hpp
    ${magicxx_INCLUDE_DIR}/magic.hpp
    ${magicxx_INCLUDE_DIR}/magic_exception.hpp
)

set(magicxx_SOURCE_FILES
    ${magicxx_SOURCE_DIR}/src/magic.cpp
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
