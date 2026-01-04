# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Toolchain: Windows x86_64 MinGW64
# Cross-compilation configuration for Windows 64-bit using MinGW-w64 GCC.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# System configuration
# -----------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME
    Windows
)

set(CMAKE_SYSTEM_PROCESSOR
    x86_64
)

set(CPACK_SYSTEM_NAME
    windows-x86_64
)

set(TARGET_OS
    x86_64-w64-mingw32
)

# -----------------------------------------------------------------------------
# External dependencies configuration
# -----------------------------------------------------------------------------

set(GNURX_TARGET
    ${TARGET_OS}
)

set(FILE_TARGET
    x86_64-pc-windows
)

# -----------------------------------------------------------------------------
# Compiler configuration
# -----------------------------------------------------------------------------

set(CMAKE_C_COMPILER
    x86_64-w64-mingw32-gcc
)

set(CMAKE_C_COMPILER_TARGET
    ${TARGET_OS}
)

set(CMAKE_CXX_COMPILER
    x86_64-w64-mingw32-g++
)

set(CMAKE_CXX_COMPILER_TARGET
    ${TARGET_OS}
)

# -----------------------------------------------------------------------------
# Search path configuration
# -----------------------------------------------------------------------------

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM
    NEVER
)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY
    ONLY
)

set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE
    ONLY
)

# -----------------------------------------------------------------------------
# Compiler and linker flags
# -----------------------------------------------------------------------------

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wfatal-errors
)
