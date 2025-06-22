# SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.30.8)

project(magicxx_tests LANGUAGES CXX)

set(magicxx_tests_SOURCE_FILES
    magic_check_test.cpp
    magic_compile_test.cpp
    magic_flags_test.cpp
    magic_identify_file_test.cpp
    magic_identify_files_test.cpp
    magic_load_database_file_test.cpp
    magic_open_close_test.cpp
    magic_parameters_test.cpp
    magic_special_members_test.cpp
    magic_to_string_test.cpp
    magic_version_test.cpp
)

include(GoogleTest)

if (BUILD_MAGICXX_SHARED_LIB)
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

    target_compile_definitions(magicxx_tests
        PRIVATE
            DEFAULT_DATABASE_FILE="${magicxx_DEFAULT_DATABASE_FILE}"
    )

    target_link_libraries(magicxx_tests
        PRIVATE
            GTest::gtest_main
            recognition::magicxx
    )

    gtest_discover_tests(magicxx_tests)
endif()

if (BUILD_MAGICXX_STATIC_LIB)
    add_executable(magicxx_tests_static)

    set_target_properties(magicxx_tests_static PROPERTIES
        CXX_STANDARD 23
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
    )

    target_sources(magicxx_tests_static
        PRIVATE
            ${magicxx_tests_SOURCE_FILES}
    )

    target_compile_definitions(magicxx_tests_static
        PRIVATE
            DEFAULT_DATABASE_FILE="${magicxx_DEFAULT_DATABASE_FILE}"
    )

    target_link_libraries(magicxx_tests_static
        PRIVATE
            GTest::gtest_main
            recognition::magicxx_static
    )

    gtest_discover_tests(magicxx_tests_static)
endif(BUILD_MAGICXX_STATIC_LIB)
