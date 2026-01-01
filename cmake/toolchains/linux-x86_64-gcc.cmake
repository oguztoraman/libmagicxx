# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

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

set(GNURX_TARGET
    ${TARGET_OS}
)

set(FILE_TARGET
    ${TARGET_OS}
)

set(CMAKE_C_COMPILER
    gcc
)

set(CMAKE_C_COMPILER_TARGET
    ${TARGET_OS}
)

set(CMAKE_CXX_COMPILER
    g++
)

set(CMAKE_CXX_COMPILER_TARGET
    ${TARGET_OS}
)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM
    NEVER
)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY
    ONLY
)

set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE
    ONLY
)

add_link_options(
    -fuse-ld=lld
)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wfatal-errors
)
