# SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.21.0)

project(magicxx_tests LANGUAGES CXX)

set(magicxx_tests_SOURCE_FILES
    magic_open_close_test.cpp
    magic_special_members_test.cpp
    magic_load_database_file_test.cpp
    magic_flags_test.cpp
    magic_parameters_test.cpp
    magic_version_test.cpp
    magic_check_test.cpp
    magic_compile_test.cpp
    magic_identify_file_test.cpp
    magic_file_concepts_test.cpp
)

enable_testing()

include(GoogleTest)

add_executable(magicxx_tests ${magicxx_tests_SOURCE_FILES})

set_target_properties(magicxx_tests PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
    INCLUDE_DIRECTORIES ${magicxx_INCLUDE_DIR}
    LINK_LIBRARIES "magicxx;GTest::gtest_main;$<$<CXX_COMPILER_ID:Clang>:c++>"
    COMPILE_OPTIONS "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wfatal-errors;$<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>>"
)

gtest_discover_tests(magicxx_tests)
