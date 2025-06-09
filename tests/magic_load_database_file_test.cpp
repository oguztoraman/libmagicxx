/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace recognition;

struct magic_load_database_file_test : public ::testing::Test {
protected:
    std::filesystem::path m_test_dir = std::filesystem::temp_directory_path()
                                     / "magicxx_test";
    std::filesystem::path m_invalid_database = m_test_dir / "invalid_database";
    std::filesystem::path m_empty_path;
    magic                 m_opened_magic{};

    void SetUp() override
    {
        std::filesystem::create_directory(m_test_dir);
        std::ofstream file{m_invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        m_opened_magic.open(magic::flags::mime);
    }

    void TearDown() override
    {
        std::filesystem::remove(m_invalid_database);
        std::filesystem::remove(m_test_dir);
    }
};

TEST_F(magic_load_database_file_test, closed_magic_load_default_database)
{
    EXPECT_THROW(magic{}.load_database_file(), magic_is_closed);
}

TEST_F(magic_load_database_file_test, opened_magic_load_empty_path)
{
    EXPECT_THROW(m_opened_magic.load_database_file(m_empty_path), empty_path);
}

TEST_F(magic_load_database_file_test, opened_magic_load_directory)
{
    EXPECT_THROW(
        m_opened_magic.load_database_file(m_test_dir),
        path_is_not_regular_file
    );
}

TEST_F(magic_load_database_file_test, opened_magic_load_invalid_database)
{
    EXPECT_THROW(
        m_opened_magic.load_database_file(m_invalid_database),
        magic_load_database_file_error
    );
}

TEST_F(magic_load_database_file_test, opened_magic_load_default_database)
{
    EXPECT_FALSE(m_opened_magic.is_valid());
    m_opened_magic.load_database_file(DEFAULT_DATABASE_FILE);
    EXPECT_TRUE(m_opened_magic.is_valid());
}
