# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
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

set(FILE_HOST
    x86_64-pc-linux-gnu
)

set(CMAKE_C_COMPILER
    clang
)

set(CMAKE_CXX_COMPILER
    clang++
)

add_link_options(
    -fuse-ld=lld
    -stdlib=libc++
)

add_compile_options(
    -Wall -Wextra -Wpedantic -Wfatal-errors -stdlib=libc++
)
