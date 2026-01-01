/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace recognition;

struct magic_special_members_test : testing::Test {
protected:
    magic_special_members_test()
    {
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        std::ofstream file{m_invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        EXPECT_TRUE(std::filesystem::exists(m_invalid_database, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~magic_special_members_test()
    {
        std::filesystem::remove(m_invalid_database, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "magic_special_members_test"
    };
    std::filesystem::path m_invalid_database{m_test_dir / "invalid_database"};
    std::filesystem::path m_nonexistent_database{
        m_test_dir / "nonexistent_database"
    };
};

TEST_F(magic_special_members_test, default_construct_magic)
{
    magic default_magic;
    EXPECT_FALSE(default_magic.is_open());
    EXPECT_FALSE(default_magic.is_database_loaded());
    EXPECT_FALSE(default_magic.is_valid());
    EXPECT_FALSE(default_magic);
}

TEST_F(magic_special_members_test, construct_magic_from_empty_path)
{
    EXPECT_THROW(magic(magic::flags::mime, m_empty_path), empty_path);
}

TEST_F(magic_special_members_test, construct_magic_from_empty_path_noexcept)
{
    magic opened_magic_without_database{
        magic::flags::mime,
        std::nothrow,
        m_empty_path
    };
    EXPECT_TRUE(opened_magic_without_database.is_open());
    EXPECT_FALSE(opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(opened_magic_without_database.is_valid());
    EXPECT_FALSE(opened_magic_without_database);
}

TEST_F(magic_special_members_test, construct_magic_from_nonexisting_database)
{
    EXPECT_THROW(
        magic(magic::flags::mime, m_nonexistent_database),
        path_does_not_exist
    );
}

TEST_F(
    magic_special_members_test,
    construct_magic_from_nonexisting_database_noexcept
)
{
    magic opened_magic_without_database{
        magic::flags::mime,
        std::nothrow,
        m_nonexistent_database
    };
    EXPECT_TRUE(opened_magic_without_database.is_open());
    EXPECT_FALSE(opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(opened_magic_without_database.is_valid());
    EXPECT_FALSE(opened_magic_without_database);
}

TEST_F(magic_special_members_test, construct_magic_from_directory)
{
    EXPECT_THROW(
        magic(magic::flags::mime, m_test_dir),
        path_is_not_regular_file
    );
}

TEST_F(magic_special_members_test, construct_magic_from_directory_noexcept)
{
    magic opened_magic_without_database{
        magic::flags::mime,
        std::nothrow,
        m_test_dir
    };
    EXPECT_TRUE(opened_magic_without_database.is_open());
    EXPECT_FALSE(opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(opened_magic_without_database.is_valid());
    EXPECT_FALSE(opened_magic_without_database);
}

TEST_F(magic_special_members_test, construct_magic_from_invalid_database)
{
    EXPECT_THROW(
        magic(magic::flags::mime, m_invalid_database),
        magic_load_database_file_error
    );
}

TEST_F(
    magic_special_members_test,
    construct_magic_from_invalid_database_noexcept
)
{
    magic opened_magic_without_database{
        magic::flags::mime,
        std::nothrow,
        m_invalid_database
    };
    EXPECT_TRUE(opened_magic_without_database.is_open());
    EXPECT_FALSE(opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(opened_magic_without_database.is_valid());
    EXPECT_FALSE(opened_magic_without_database);
}

TEST_F(magic_special_members_test, construct_magic_from_valid_database)
{
    magic valid_magic{magic::flags::mime, std::nothrow, m_valid_database};
    EXPECT_TRUE(valid_magic.is_open());
    EXPECT_TRUE(valid_magic.is_database_loaded());
    EXPECT_TRUE(valid_magic.is_valid());
    EXPECT_TRUE(valid_magic);
}

TEST_F(magic_special_members_test, construct_magic_from_valid_database_noexcept)
{
    magic valid_magic{magic::flags::mime, std::nothrow, m_valid_database};
    EXPECT_TRUE(valid_magic.is_open());
    EXPECT_TRUE(valid_magic.is_database_loaded());
    EXPECT_TRUE(valid_magic.is_valid());
    EXPECT_TRUE(valid_magic);
}

TEST_F(magic_special_members_test, move_construct_magic_from_closed_magic)
{
    magic closed_magic;
    EXPECT_FALSE(closed_magic.is_open());
    EXPECT_FALSE(closed_magic.is_database_loaded());
    EXPECT_FALSE(closed_magic.is_valid());
    EXPECT_FALSE(closed_magic);
    auto new_magic{std::move(closed_magic)};
    EXPECT_FALSE(new_magic.is_open());
    EXPECT_FALSE(new_magic.is_database_loaded());
    EXPECT_FALSE(new_magic.is_valid());
    EXPECT_FALSE(new_magic);
    EXPECT_FALSE(closed_magic.is_open());
    EXPECT_FALSE(closed_magic.is_database_loaded());
    EXPECT_FALSE(closed_magic.is_valid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(magic_special_members_test, move_construct_magic_from_opened_magic)
{
    magic opened_magic;
    EXPECT_TRUE(opened_magic.open(magic::flags::mime, std::nothrow));
    EXPECT_TRUE(opened_magic.is_open());
    EXPECT_FALSE(opened_magic.is_database_loaded());
    EXPECT_FALSE(opened_magic.is_valid());
    EXPECT_FALSE(opened_magic);
    auto new_magic{std::move(opened_magic)};
    EXPECT_TRUE(new_magic.is_open());
    EXPECT_FALSE(new_magic.is_database_loaded());
    EXPECT_FALSE(new_magic.is_valid());
    EXPECT_FALSE(new_magic);
    EXPECT_FALSE(opened_magic.is_open());
    EXPECT_FALSE(opened_magic.is_database_loaded());
    EXPECT_FALSE(opened_magic.is_valid());
    EXPECT_FALSE(opened_magic);
}

TEST_F(magic_special_members_test, move_construct_magic_from_valid_magic)
{
    magic valid_magic(magic::flags::mime, std::nothrow, m_valid_database);
    EXPECT_TRUE(valid_magic.is_open());
    EXPECT_TRUE(valid_magic.is_database_loaded());
    EXPECT_TRUE(valid_magic.is_valid());
    EXPECT_TRUE(valid_magic);
    auto new_magic{std::move(valid_magic)};
    EXPECT_TRUE(new_magic.is_open());
    EXPECT_TRUE(new_magic.is_database_loaded());
    EXPECT_TRUE(new_magic.is_valid());
    EXPECT_TRUE(new_magic);
    EXPECT_FALSE(valid_magic.is_open());
    EXPECT_FALSE(valid_magic.is_database_loaded());
    EXPECT_FALSE(valid_magic.is_valid());
    EXPECT_FALSE(valid_magic);
}

TEST_F(magic_special_members_test, move_assign_magic_from_closed_magic)
{
    magic closed_magic;
    EXPECT_FALSE(closed_magic.is_open());
    EXPECT_FALSE(closed_magic.is_database_loaded());
    EXPECT_FALSE(closed_magic.is_valid());
    EXPECT_FALSE(closed_magic);
    auto new_magic = std::move(closed_magic);
    EXPECT_FALSE(new_magic.is_open());
    EXPECT_FALSE(new_magic.is_database_loaded());
    EXPECT_FALSE(new_magic.is_valid());
    EXPECT_FALSE(new_magic);
    EXPECT_FALSE(closed_magic.is_open());
    EXPECT_FALSE(closed_magic.is_database_loaded());
    EXPECT_FALSE(closed_magic.is_valid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(magic_special_members_test, move_assign_magic_from_opened_magic)
{
    magic opened_magic;
    EXPECT_TRUE(opened_magic.open(magic::flags::mime, std::nothrow));
    EXPECT_TRUE(opened_magic.is_open());
    EXPECT_FALSE(opened_magic.is_database_loaded());
    EXPECT_FALSE(opened_magic.is_valid());
    EXPECT_FALSE(opened_magic);
    auto new_magic = std::move(opened_magic);
    EXPECT_TRUE(new_magic.is_open());
    EXPECT_FALSE(new_magic.is_database_loaded());
    EXPECT_FALSE(new_magic.is_valid());
    EXPECT_FALSE(new_magic);
    EXPECT_FALSE(opened_magic.is_open());
    EXPECT_FALSE(opened_magic.is_database_loaded());
    EXPECT_FALSE(opened_magic.is_valid());
    EXPECT_FALSE(opened_magic);
}

TEST_F(magic_special_members_test, move_assign_magic_from_valid_magic)
{
    magic valid_magic(magic::flags::mime, std::nothrow, m_valid_database);
    EXPECT_TRUE(valid_magic.is_open());
    EXPECT_TRUE(valid_magic.is_database_loaded());
    EXPECT_TRUE(valid_magic.is_valid());
    EXPECT_TRUE(valid_magic);
    auto new_magic = std::move(valid_magic);
    EXPECT_TRUE(new_magic.is_open());
    EXPECT_TRUE(new_magic.is_database_loaded());
    EXPECT_TRUE(new_magic.is_valid());
    EXPECT_TRUE(new_magic);
    EXPECT_FALSE(valid_magic.is_open());
    EXPECT_FALSE(valid_magic.is_database_loaded());
    EXPECT_FALSE(valid_magic.is_valid());
    EXPECT_FALSE(valid_magic);
}
