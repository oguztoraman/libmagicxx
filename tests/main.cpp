/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file main.cpp
 * @brief GoogleTest main entry point for libmagicxx unit tests.
 *
 * This file contains the main() function that initializes GoogleTest
 * and runs all registered test cases for the libmagicxx library.
 *
 * @section tests_overview Test Suite Overview
 *
 * The test suite is organized into the following test files:
 *
 * | Test File | Description |
 * |-----------|-------------|
 * | magic_check_test.cpp | Tests for Magic::Check() database validation |
 * | magic_compile_test.cpp | Tests for Magic::Compile() database compilation |
 * | magic_flags_test.cpp | Tests for flag getting/setting operations |
 * | magic_identify_container_test.cpp | Tests for container-based identification |
 * | magic_identify_directory_test.cpp | Tests for directory scanning |
 * | magic_identify_file_test.cpp | Tests for single file identification |
 * | magic_load_database_file_test.cpp | Tests for database loading |
 * | magic_open_close_test.cpp | Tests for open/close lifecycle |
 * | magic_parameters_test.cpp | Tests for parameter getting/setting |
 * | magic_percentage_test.cpp | Tests for Percentage utility class |
 * | magic_progress_tracker_test.cpp | Tests for ProgressTracker utility |
 * | magic_special_members_test.cpp | Tests for constructors and special members |
 * | magic_to_string_test.cpp | Tests for ToString() functions |
 * | magic_version_test.cpp | Tests for Magic::GetVersion() |
 *
 * @section tests_running Running the Tests
 *
 * Build and run tests using the test preset:
 * @code{.sh}
 * ./scripts/workflows.sh -p linux-x86_64-clang-tests
 * @endcode
 *
 * @section tests_coverage Test Coverage Strategy
 *
 * Each API function is tested for:
 * - **Normal operation**: Valid inputs and expected outputs
 * - **Error handling**: Invalid inputs, edge cases, exceptions
 * - **State transitions**: Closed, opened, database loaded states
 * - **Noexcept variants**: Both throwing and non-throwing overloads
 *
 * @see Recognition::Magic
 * @see Recognition::MagicException
 */

#include <gtest/gtest.h>

/**
 * @brief Main entry point for the test suite.
 *
 * Initializes GoogleTest framework and runs all discovered tests.
 *
 * @param[in] argc Argument count from command line.
 * @param[in] argv Argument values from command line.
 *
 * @return 0 if all tests pass, non-zero otherwise.
 */
auto main(int argc, char** argv) -> int
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
