/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace Recognition;

struct MagicSpecialMembersTest : testing::Test {
protected:
    MagicSpecialMembersTest()
    {
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        std::ofstream file{m_invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        EXPECT_TRUE(std::filesystem::exists(m_invalid_database, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~MagicSpecialMembersTest()
    {
        std::filesystem::remove(m_invalid_database, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "MagicSpecialMembersTest"
    };
    std::filesystem::path m_invalid_database{m_test_dir / "invalid_database"};
    std::filesystem::path m_nonexistent_database{
        m_test_dir / "nonexistent_database"
    };
};

TEST_F(MagicSpecialMembersTest, default_construct_magic)
{
    Magic default_magic;
    EXPECT_FALSE(default_magic.IsOpen());
    EXPECT_FALSE(default_magic.IsDatabaseLoaded());
    EXPECT_FALSE(default_magic.IsValid());
    EXPECT_FALSE(default_magic);
}

TEST_F(MagicSpecialMembersTest, construct_magic_from_empty_path)
{
    EXPECT_THROW(Magic(Magic::Flags::Mime, m_empty_path), EmptyPath);
}

TEST_F(MagicSpecialMembersTest, construct_magic_from_empty_path_noexcept)
{
    Magic opened_magic_without_database{
        Magic::Flags::Mime,
        std::nothrow,
        m_empty_path
    };
    EXPECT_TRUE(opened_magic_without_database.IsOpen());
    EXPECT_FALSE(opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(opened_magic_without_database.IsValid());
    EXPECT_FALSE(opened_magic_without_database);
}

TEST_F(MagicSpecialMembersTest, construct_magic_from_nonexisting_database)
{
    EXPECT_THROW(
        Magic(Magic::Flags::Mime, m_nonexistent_database),
        PathDoesNotExist
    );
}

TEST_F(
    MagicSpecialMembersTest,
    construct_magic_from_nonexisting_database_noexcept
)
{
    Magic opened_magic_without_database{
        Magic::Flags::Mime,
        std::nothrow,
        m_nonexistent_database
    };
    EXPECT_TRUE(opened_magic_without_database.IsOpen());
    EXPECT_FALSE(opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(opened_magic_without_database.IsValid());
    EXPECT_FALSE(opened_magic_without_database);
}

TEST_F(MagicSpecialMembersTest, construct_magic_from_directory)
{
    EXPECT_THROW(
        Magic(Magic::Flags::Mime, m_test_dir),
        PathIsNotRegularFile
    );
}

TEST_F(MagicSpecialMembersTest, construct_magic_from_directory_noexcept)
{
    Magic opened_magic_without_database{
        Magic::Flags::Mime,
        std::nothrow,
        m_test_dir
    };
    EXPECT_TRUE(opened_magic_without_database.IsOpen());
    EXPECT_FALSE(opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(opened_magic_without_database.IsValid());
    EXPECT_FALSE(opened_magic_without_database);
}

TEST_F(MagicSpecialMembersTest, construct_magic_from_invalid_database)
{
    EXPECT_THROW(
        Magic(Magic::Flags::Mime, m_invalid_database),
        MagicLoadDatabaseFileError
    );
}

TEST_F(
    MagicSpecialMembersTest,
    construct_magic_from_invalid_database_noexcept
)
{
    Magic opened_magic_without_database{
        Magic::Flags::Mime,
        std::nothrow,
        m_invalid_database
    };
    EXPECT_TRUE(opened_magic_without_database.IsOpen());
    EXPECT_FALSE(opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(opened_magic_without_database.IsValid());
    EXPECT_FALSE(opened_magic_without_database);
}

TEST_F(MagicSpecialMembersTest, construct_magic_from_valid_database)
{
    Magic valid_magic{Magic::Flags::Mime, std::nothrow, m_valid_database};
    EXPECT_TRUE(valid_magic.IsOpen());
    EXPECT_TRUE(valid_magic.IsDatabaseLoaded());
    EXPECT_TRUE(valid_magic.IsValid());
    EXPECT_TRUE(valid_magic);
}

TEST_F(MagicSpecialMembersTest, construct_magic_from_valid_database_noexcept)
{
    Magic valid_magic{Magic::Flags::Mime, std::nothrow, m_valid_database};
    EXPECT_TRUE(valid_magic.IsOpen());
    EXPECT_TRUE(valid_magic.IsDatabaseLoaded());
    EXPECT_TRUE(valid_magic.IsValid());
    EXPECT_TRUE(valid_magic);
}

TEST_F(MagicSpecialMembersTest, move_construct_magic_from_closed_magic)
{
    Magic closed_magic;
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
    auto new_magic{std::move(closed_magic)};
    EXPECT_FALSE(new_magic.IsOpen());
    EXPECT_FALSE(new_magic.IsDatabaseLoaded());
    EXPECT_FALSE(new_magic.IsValid());
    EXPECT_FALSE(new_magic);
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(MagicSpecialMembersTest, move_construct_magic_from_opened_magic)
{
    Magic opened_magic;
    EXPECT_TRUE(opened_magic.Open(Magic::Flags::Mime, std::nothrow));
    EXPECT_TRUE(opened_magic.IsOpen());
    EXPECT_FALSE(opened_magic.IsDatabaseLoaded());
    EXPECT_FALSE(opened_magic.IsValid());
    EXPECT_FALSE(opened_magic);
    auto new_magic{std::move(opened_magic)};
    EXPECT_TRUE(new_magic.IsOpen());
    EXPECT_FALSE(new_magic.IsDatabaseLoaded());
    EXPECT_FALSE(new_magic.IsValid());
    EXPECT_FALSE(new_magic);
    EXPECT_FALSE(opened_magic.IsOpen());
    EXPECT_FALSE(opened_magic.IsDatabaseLoaded());
    EXPECT_FALSE(opened_magic.IsValid());
    EXPECT_FALSE(opened_magic);
}

TEST_F(MagicSpecialMembersTest, move_construct_magic_from_valid_magic)
{
    Magic valid_magic(Magic::Flags::Mime, std::nothrow, m_valid_database);
    EXPECT_TRUE(valid_magic.IsOpen());
    EXPECT_TRUE(valid_magic.IsDatabaseLoaded());
    EXPECT_TRUE(valid_magic.IsValid());
    EXPECT_TRUE(valid_magic);
    auto new_magic{std::move(valid_magic)};
    EXPECT_TRUE(new_magic.IsOpen());
    EXPECT_TRUE(new_magic.IsDatabaseLoaded());
    EXPECT_TRUE(new_magic.IsValid());
    EXPECT_TRUE(new_magic);
    EXPECT_FALSE(valid_magic.IsOpen());
    EXPECT_FALSE(valid_magic.IsDatabaseLoaded());
    EXPECT_FALSE(valid_magic.IsValid());
    EXPECT_FALSE(valid_magic);
}

TEST_F(MagicSpecialMembersTest, move_assign_magic_from_closed_magic)
{
    Magic closed_magic;
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
    auto new_magic = std::move(closed_magic);
    EXPECT_FALSE(new_magic.IsOpen());
    EXPECT_FALSE(new_magic.IsDatabaseLoaded());
    EXPECT_FALSE(new_magic.IsValid());
    EXPECT_FALSE(new_magic);
    EXPECT_FALSE(closed_magic.IsOpen());
    EXPECT_FALSE(closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(closed_magic.IsValid());
    EXPECT_FALSE(closed_magic);
}

TEST_F(MagicSpecialMembersTest, move_assign_magic_from_opened_magic)
{
    Magic opened_magic;
    EXPECT_TRUE(opened_magic.Open(Magic::Flags::Mime, std::nothrow));
    EXPECT_TRUE(opened_magic.IsOpen());
    EXPECT_FALSE(opened_magic.IsDatabaseLoaded());
    EXPECT_FALSE(opened_magic.IsValid());
    EXPECT_FALSE(opened_magic);
    auto new_magic = std::move(opened_magic);
    EXPECT_TRUE(new_magic.IsOpen());
    EXPECT_FALSE(new_magic.IsDatabaseLoaded());
    EXPECT_FALSE(new_magic.IsValid());
    EXPECT_FALSE(new_magic);
    EXPECT_FALSE(opened_magic.IsOpen());
    EXPECT_FALSE(opened_magic.IsDatabaseLoaded());
    EXPECT_FALSE(opened_magic.IsValid());
    EXPECT_FALSE(opened_magic);
}

TEST_F(MagicSpecialMembersTest, move_assign_magic_from_valid_magic)
{
    Magic valid_magic(Magic::Flags::Mime, std::nothrow, m_valid_database);
    EXPECT_TRUE(valid_magic.IsOpen());
    EXPECT_TRUE(valid_magic.IsDatabaseLoaded());
    EXPECT_TRUE(valid_magic.IsValid());
    EXPECT_TRUE(valid_magic);
    auto new_magic = std::move(valid_magic);
    EXPECT_TRUE(new_magic.IsOpen());
    EXPECT_TRUE(new_magic.IsDatabaseLoaded());
    EXPECT_TRUE(new_magic.IsValid());
    EXPECT_TRUE(new_magic);
    EXPECT_FALSE(valid_magic.IsOpen());
    EXPECT_FALSE(valid_magic.IsDatabaseLoaded());
    EXPECT_FALSE(valid_magic.IsValid());
    EXPECT_FALSE(valid_magic);
}
