# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Toolchain: Linux x86_64 Clang
# Cross-compilation configuration for Linux 64-bit using Clang/LLVM toolchain.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# System configuration
# -----------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME
    Linux
)

set(CMAKE_SYSTEM_PROCESSOR
    x86_64
)

set(CPACK_SYSTEM_NAME
    linux-x86_64
)

set(TARGET_OS
    x86_64-pc-linux-gnu
)

# -----------------------------------------------------------------------------
# External dependencies configuration
# -----------------------------------------------------------------------------

set(GNURX_TARGET
    ${TARGET_OS}
)

set(GNURX_CFLAGS
    --target=${TARGET_OS}
)

set(FILE_TARGET
    ${TARGET_OS}
)

set(FILE_CFLAGS
    --target=${FILE_TARGET}
)

# -----------------------------------------------------------------------------
# Compiler configuration
# -----------------------------------------------------------------------------

set(CMAKE_C_COMPILER
    clang
)

set(CMAKE_C_COMPILER_TARGET
    ${TARGET_OS}
)

set(CMAKE_CXX_COMPILER
    clang++
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

add_link_options(
    -fuse-ld=lld
    -stdlib=libc++
)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wfatal-errors
    -stdlib=libc++
)
