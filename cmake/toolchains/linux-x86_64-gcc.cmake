# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

set(CMAKE_SYSTEM_NAME
    Linux
)

set(CMAKE_SYSTEM_PROCESSOR
    x86_64
)

set(FILE_HOST
    x86_64-pc-linux-gnu
)

set(CMAKE_C_COMPILER
    gcc
)

set(CMAKE_CXX_COMPILER
    g++
)

add_link_options(
    -fuse-ld=mold
)

add_compile_options(
    -Wall -Wextra -Wpedantic -Wfatal-errors
)
