# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.31.6)

project(magicxx
    VERSION 9.1.0
    DESCRIPTION "A C++ wrapper library over the Magic Number Recognition Library."
    HOMEPAGE_URL "https://github.com/oguztoraman/libmagicxx"
    LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/options.cmake)
include(cmake/directories.cmake)
include(cmake/sources.cmake)
include(cmake/files.cmake)
include(cmake/dependencies.cmake)
include(cmake/targets.cmake)
include(cmake/package_config.cmake)
include(cmake/install.cmake)
include(cmake/uninstall.cmake)
include(cmake/cpack.cmake)
include(cmake/format_source_code.cmake)
include(cmake/generate_default_database_files.cmake)

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
