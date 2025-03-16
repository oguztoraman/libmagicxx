# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.30.8)

project(magicxx_examples LANGUAGES CXX)

set(magicxx_examples_SOURCE_FILES
    magic_examples.cpp
)

add_executable(magicxx_examples)

set_target_properties(magicxx_examples PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

target_sources(magicxx_examples
    PRIVATE
        ${magicxx_examples_SOURCE_FILES}
)

target_include_directories(magicxx_examples
    PRIVATE
        ${magicxx_INCLUDE_DIR}
)

target_link_libraries(magicxx_examples
    PRIVATE
        magicxx
)