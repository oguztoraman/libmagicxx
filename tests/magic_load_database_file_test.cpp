/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace Recognition;

struct MagicLoadDatabaseFileTest : testing::Test {
protected:
    MagicLoadDatabaseFileTest()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .Open(Magic::Flags::Mime, std::nothrow));
        EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        std::ofstream file{m_invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        EXPECT_TRUE(std::filesystem::exists(m_invalid_database, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~MagicLoadDatabaseFileTest()
    {
        std::filesystem::remove(m_invalid_database, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "MagicLoadDatabaseFileTest"
    };
    std::filesystem::path m_invalid_database{m_test_dir / "invalid_database"};
    std::filesystem::path m_nonexistent_database{
        m_test_dir / "nonexistent_database"
    };
    Magic m_closed_magic{};
    Magic m_opened_magic_without_database;
};

TEST_F(MagicLoadDatabaseFileTest, closed_magic)
{
    EXPECT_THROW(
        m_closed_magic.LoadDatabaseFile(m_valid_database),
        MagicIsClosed
    );
}

TEST_F(MagicLoadDatabaseFileTest, closed_magic_noexcept)
{
    EXPECT_FALSE(
        m_closed_magic.LoadDatabaseFile(std::nothrow, m_valid_database)
    );
    EXPECT_FALSE(m_closed_magic.IsDatabaseLoaded());
    EXPECT_FALSE(m_closed_magic.IsValid());
    EXPECT_FALSE(m_closed_magic);
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_empty_path
)
{
    EXPECT_THROW(
        m_opened_magic_without_database.LoadDatabaseFile(m_empty_path),
        EmptyPath
    );
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_empty_path_noexcept
)
{
    EXPECT_FALSE(m_opened_magic_without_database
                     .LoadDatabaseFile(std::nothrow, m_empty_path));
    EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(m_opened_magic_without_database.IsValid());
    EXPECT_FALSE(m_opened_magic_without_database);
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_nonexistent_database
)
{
    EXPECT_THROW(
        m_opened_magic_without_database.LoadDatabaseFile(
            m_nonexistent_database
        ),
        PathDoesNotExist
    );
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_nonexistent_database_noexcept
)
{
    EXPECT_FALSE(m_opened_magic_without_database
                     .LoadDatabaseFile(std::nothrow, m_nonexistent_database));
    EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(m_opened_magic_without_database.IsValid());
    EXPECT_FALSE(m_opened_magic_without_database);
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_directory
)
{
    EXPECT_THROW(
        m_opened_magic_without_database.LoadDatabaseFile(m_test_dir),
        PathIsNotRegularFile
    );
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_directory_noexcept
)
{
    EXPECT_FALSE(m_opened_magic_without_database
                     .LoadDatabaseFile(std::nothrow, m_test_dir));
    EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(m_opened_magic_without_database.IsValid());
    EXPECT_FALSE(m_opened_magic_without_database);
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_invalid_database
)
{
    EXPECT_THROW(
        m_opened_magic_without_database.LoadDatabaseFile(m_invalid_database),
        MagicLoadDatabaseFileError
    );
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_invalid_database_noexcept
)
{
    EXPECT_FALSE(m_opened_magic_without_database
                     .LoadDatabaseFile(std::nothrow, m_invalid_database));
    EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(m_opened_magic_without_database.IsValid());
    EXPECT_FALSE(m_opened_magic_without_database);
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_valid_database
)
{
    m_opened_magic_without_database.LoadDatabaseFile(m_valid_database);
    EXPECT_TRUE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_TRUE(m_opened_magic_without_database.IsValid());
    EXPECT_TRUE(m_opened_magic_without_database);
}

TEST_F(
    MagicLoadDatabaseFileTest,
    opened_magic_without_database_load_valid_database_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .LoadDatabaseFile(std::nothrow, m_valid_database));
    EXPECT_TRUE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_TRUE(m_opened_magic_without_database.IsValid());
    EXPECT_TRUE(m_opened_magic_without_database);
}

TEST_F(MagicLoadDatabaseFileTest, load_database_files_multiple_times)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .LoadDatabaseFile(std::nothrow, m_valid_database));
    EXPECT_TRUE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_TRUE(m_opened_magic_without_database.IsValid());
    EXPECT_TRUE(m_opened_magic_without_database);

    EXPECT_FALSE(m_opened_magic_without_database
                     .LoadDatabaseFile(std::nothrow, m_invalid_database));
    EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_FALSE(m_opened_magic_without_database.IsValid());
    EXPECT_FALSE(m_opened_magic_without_database);

    EXPECT_TRUE(m_opened_magic_without_database
                    .LoadDatabaseFile(std::nothrow, m_valid_database));
    EXPECT_TRUE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_TRUE(m_opened_magic_without_database.IsValid());
    EXPECT_TRUE(m_opened_magic_without_database);

    EXPECT_TRUE(m_opened_magic_without_database
                    .LoadDatabaseFile(std::nothrow, m_valid_database));
    EXPECT_TRUE(m_opened_magic_without_database.IsDatabaseLoaded());
    EXPECT_TRUE(m_opened_magic_without_database.IsValid());
    EXPECT_TRUE(m_opened_magic_without_database);
}
