# SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.30.8)

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

include(GoogleTest)

add_executable(magicxx_tests)

set_target_properties(magicxx_tests PROPERTIES
    CXX_STANDARD 23
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

target_sources(magicxx_tests
    PRIVATE
        ${magicxx_tests_SOURCE_FILES}
)

target_include_directories(magicxx_tests
    PRIVATE
        ${magicxx_INCLUDE_DIR}
)

target_link_libraries(magicxx_tests
    PRIVATE
        magicxx GTest::gtest_main
)

gtest_discover_tests(magicxx_tests)
