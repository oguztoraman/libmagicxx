/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace recognition;

struct magic_load_database_file_test : public ::testing::Test {
protected:
    std::filesystem::path test_dir = std::filesystem::temp_directory_path()
                                   / "magicxx_test";
    std::filesystem::path invalid_database = test_dir / "invalid_database";
    magic                 opened_magic{};

    void SetUp() override
    {
        std::filesystem::create_directory(test_dir);
        std::ofstream file{invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        opened_magic.open(magic::flags::mime);
    }

    void TearDown() override
    {
        std::filesystem::remove(invalid_database);
        std::filesystem::remove(test_dir);
    }
};

TEST_F(magic_load_database_file_test, closed_magic_load_default_database)
{
    EXPECT_THROW(magic{}.load_database_file(), magic_is_closed);
}

TEST_F(magic_load_database_file_test, opened_magic_load_empty_path)
{
    EXPECT_THROW(opened_magic.load_database_file({}), empty_path);
}

TEST_F(magic_load_database_file_test, opened_magic_load_directory)
{
    EXPECT_THROW(opened_magic.load_database_file(test_dir), invalid_path);
}

TEST_F(magic_load_database_file_test, opened_magic_load_invalid_database)
{
    EXPECT_THROW(
        opened_magic.load_database_file(invalid_database),
        magic_load_error
    );
}

TEST_F(magic_load_database_file_test, opened_magic_load_default_database)
{
    EXPECT_FALSE(opened_magic.is_valid());
    opened_magic.load_database_file(DEFAULT_DATABASE_FILE);
    EXPECT_TRUE(opened_magic.is_valid());
}
