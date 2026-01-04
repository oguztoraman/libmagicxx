# SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com>
# SPDX-License-Identifier: LGPL-3.0-only

# -----------------------------------------------------------------------------
# Unit tests for Libmagicxx
# GoogleTest-based test suite for verifying library functionality.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.31.6)

project(magicxx_tests
    DESCRIPTION "Unit tests for Libmagicxx."
    LANGUAGES CXX
)

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------

set(magicxx_tests_SOURCE_FILES
    magic_check_test.cpp
    magic_compile_test.cpp
    magic_flags_test.cpp
    magic_identify_container_test.cpp
    magic_identify_directory_test.cpp
    magic_identify_file_test.cpp
    magic_load_database_file_test.cpp
    magic_open_close_test.cpp
    magic_parameters_test.cpp
    magic_percentage_test.cpp
    magic_progress_tracker_test.cpp
    magic_special_members_test.cpp
    magic_to_string_test.cpp
    magic_version_test.cpp
    main.cpp
)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

include(GoogleTest)

# -----------------------------------------------------------------------------
# Helper function to configure a test executable
# -----------------------------------------------------------------------------
function(configure_test_executable target_name library_target)
    add_executable(${target_name})

    target_sources(${target_name}
        PRIVATE
            ${magicxx_tests_SOURCE_FILES}
    )

    target_compile_features(${target_name}
        PRIVATE
            cxx_std_23
    )

    set_target_properties(${target_name} PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
    )

    target_compile_definitions(${target_name}
        PRIVATE
            MAGIC_DEFAULT_DATABASE_FILE="${magicxx_DEFAULT_DATABASE_FILE}"
    )

    target_link_libraries(${target_name}
        PRIVATE
            GTest::gtest
            ${library_target}
    )

    gtest_discover_tests(${target_name})
endfunction()

# -----------------------------------------------------------------------------
# Build test executables
# -----------------------------------------------------------------------------
if(BUILD_MAGICXX_SHARED_LIB)
    configure_test_executable(magicxx_tests Recognition::Magicxx)
endif()

if(BUILD_MAGICXX_STATIC_LIB)
    configure_test_executable(magicxx_tests_static Recognition::MagicxxStatic)
endif()
