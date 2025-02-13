# SPDX-FileCopyrightText: Copyright (c) 2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.21.0)

project(magicxx_examples LANGUAGES CXX)

set(magicxx_examples_SOURCE_FILES
    magic_examples.cpp
)

add_executable(magicxx_examples ${magicxx_examples_SOURCE_FILES})

set_target_properties(magicxx_examples PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    INCLUDE_DIRECTORIES ${magicxx_INCLUDE_DIR}
    LINK_LIBRARIES "magicxx;$<$<CXX_COMPILER_ID:Clang>:c++>"
    COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wfatal-errors;$<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>>"
)
