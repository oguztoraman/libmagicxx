/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace Recognition;

struct MagicCheckTest : testing::Test {
protected:
    MagicCheckTest()
    {
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        std::ofstream file{m_invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        EXPECT_TRUE(std::filesystem::exists(m_invalid_database, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~MagicCheckTest()
    {
        std::filesystem::remove(m_invalid_database, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "MagicCheckTest"
    };
    std::filesystem::path m_invalid_database{m_test_dir / "invalid_database"};
    std::filesystem::path m_nonexistent_database{
        m_test_dir / "nonexistent_database"
    };
};

TEST_F(MagicCheckTest, check_empty_path)
{
    EXPECT_FALSE(Magic::Check(m_empty_path));
}

TEST_F(MagicCheckTest, check_nonexistent_database)
{
    EXPECT_FALSE(Magic::Check(m_nonexistent_database));
}

TEST_F(MagicCheckTest, check_directory)
{
    EXPECT_FALSE(Magic::Check(m_test_dir));
}

TEST_F(MagicCheckTest, check_invalid_database)
{
    EXPECT_FALSE(Magic::Check(m_invalid_database));
}

TEST_F(MagicCheckTest, check_valid_database)
{
    EXPECT_TRUE(Magic::Check(m_valid_database));
}
