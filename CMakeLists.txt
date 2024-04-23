# SPDX-FileCopyrightText: Copyright (c) 2022-2024 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.21.0)

project(magic++
    VERSION 3.1.0
    DESCRIPTION "A C++ wrapper library over the Magic Number Recognition Library."
    HOMEPAGE_URL "https://github.com/oguztoraman/magic-cpp"
    LANGUAGES CXX
)

set(magic_INCLUDE_DIR
    ${magic++_SOURCE_DIR}/file/src
)

set(magic_LIBRARY
    ${magic_INCLUDE_DIR}/.libs/libmagic.so
)

if (NOT EXISTS ${magic_LIBRARY})
    message(FATAL_ERROR "First, you need to run the setup.sh bash script.")
endif()

set(magic++_INCLUDE_DIR
    ${magic++_SOURCE_DIR}/inc
)

set(magic++_HEADER_FILES
    ${magic++_INCLUDE_DIR}/file_concepts.hpp
    ${magic++_INCLUDE_DIR}/magic.hpp
    ${magic++_INCLUDE_DIR}/magic_exception.hpp
)

set(magic++_SOURCE_FILES
    ${magic++_SOURCE_DIR}/src/magic.cpp
)

add_library(magic++ SHARED)

set_target_properties(magic++ PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    SOURCES ${magic++_SOURCE_FILES}
    VERSION ${magic++_VERSION}
    SOVERSION ${magic++_VERSION_MAJOR}
    LINK_LIBRARIES ${magic_LIBRARY}
    COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wfatal-errors>"
)

target_include_directories(magic++
    PUBLIC ${magic_INCLUDE_DIR}
    PUBLIC ${magic++_INCLUDE_DIR}
)
