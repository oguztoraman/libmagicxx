/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace recognition;

struct magic_check_test : testing::Test {
protected:
    magic_check_test()
    {
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        std::ofstream file{m_invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        EXPECT_TRUE(std::filesystem::exists(m_invalid_database, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~magic_check_test()
    {
        std::filesystem::remove(m_invalid_database, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "magic_check_test"
    };
    std::filesystem::path m_invalid_database{m_test_dir / "invalid_database"};
    std::filesystem::path m_nonexistent_database{
        m_test_dir / "nonexistent_database"
    };
};

TEST_F(magic_check_test, check_empty_path)
{
    EXPECT_FALSE(magic::check(m_empty_path));
}

TEST_F(magic_check_test, check_nonexistent_database)
{
    EXPECT_FALSE(magic::check(m_nonexistent_database));
}

TEST_F(magic_check_test, check_directory)
{
    EXPECT_FALSE(magic::check(m_test_dir));
}

TEST_F(magic_check_test, check_invalid_database)
{
    EXPECT_FALSE(magic::check(m_invalid_database));
}

TEST_F(magic_check_test, check_valid_database)
{
    EXPECT_TRUE(magic::check(m_valid_database));
}
