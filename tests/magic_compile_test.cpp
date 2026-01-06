/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_compile_test.cpp
 * @brief Unit tests for Magic::Compile() static method.
 *
 * Tests the database compilation functionality that compiles
 * a magic source file into a binary .mgc database file.
 *
 * @section compile_test_cases Test Cases
 *
 * | Test Case | Description |
 * |-----------|-------------|
 * | compile_empty_path | Empty path returns false |
 * | compile_nonexistent_database | Non-existent file returns false |
 * | compile_directory | Directory path returns false |
 * | compile_invalid_database | Invalid source returns false |
 * | compile_valid_database | Valid source compiles successfully |
 *
 * @see Magic::Compile()
 */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace Recognition;

struct MagicCompileTest : testing::Test {
protected:
    MagicCompileTest()
    {
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        std::ofstream file{m_invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        EXPECT_TRUE(std::filesystem::exists(m_invalid_database, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~MagicCompileTest()
    {
        std::filesystem::remove(m_invalid_database, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "MagicCompileTest"
    };
    std::filesystem::path m_invalid_database{m_test_dir / "invalid_database"};
    std::filesystem::path m_nonexistent_database{
        m_test_dir / "nonexistent_database"
    };
};

TEST_F(MagicCompileTest, compile_empty_path)
{
    EXPECT_FALSE(Magic::Compile(m_empty_path));
}

TEST_F(MagicCompileTest, compile_nonexistent_database)
{
    EXPECT_FALSE(Magic::Compile(m_nonexistent_database));
}

TEST_F(MagicCompileTest, compile_directory)
{
    EXPECT_FALSE(Magic::Compile(m_test_dir));
}

TEST_F(MagicCompileTest, compile_invalid_database)
{
    EXPECT_FALSE(Magic::Compile(m_invalid_database));
}

TEST_F(MagicCompileTest, compile_valid_database)
{
    EXPECT_TRUE(Magic::Compile(m_valid_database));
}
