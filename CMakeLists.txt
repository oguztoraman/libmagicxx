# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Libmagicxx - A modern C++23 wrapper for libmagic
#
# This is the main build configuration for libmagicxx.
#
# Build options:
#   BUILD_MAGICXX_SHARED_LIB  - Build shared library (default: ON)
#   BUILD_MAGICXX_STATIC_LIB  - Build static library (default: ON)
#   BUILD_MAGICXX_TESTS       - Build unit tests (default: OFF)
#   BUILD_MAGICXX_EXAMPLES    - Build example applications (default: OFF)
#
# Usage:
#   cmake --workflow --preset <preset-name>
#   ./scripts/workflows.sh -p <preset-name>
#
# See CMakePresets.json for available configuration presets.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)

# -----------------------------------------------------------------------------
# Prevent in-source builds
# -----------------------------------------------------------------------------
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Please use a separate build directory."
    )
endif()

project(magicxx
    VERSION 10.0.2
    DESCRIPTION "A modern C++23 wrapper for libmagic — the library that powers the Unix file command."
    HOMEPAGE_URL "https://github.com/oguztoraman/libmagicxx"
    LANGUAGES C CXX
)

# -----------------------------------------------------------------------------
# Global settings
# -----------------------------------------------------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------------
# Include CMake modules
# -----------------------------------------------------------------------------
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

# -----------------------------------------------------------------------------
# Optional components
# -----------------------------------------------------------------------------
if(BUILD_MAGICXX_TESTS)
    enable_testing()
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()

if(BUILD_MAGICXX_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_MAGICXX_DOCUMENTATION)
    add_subdirectory(documentation)
endif()

# -----------------------------------------------------------------------------
# Feature summary
# -----------------------------------------------------------------------------
feature_summary(
    WHAT ENABLED_FEATURES DISABLED_FEATURES
    DESCRIPTION "Libmagicxx build configuration:"
)
