/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace recognition;

struct magic_load_database_file_test : testing::Test {
protected:
    magic_load_database_file_test()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .open(magic::flags::mime, std::nothrow));
        EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        std::ofstream file{m_invalid_database, std::ios::trunc};
        file << "test";
        file.flush();
        EXPECT_TRUE(std::filesystem::exists(m_invalid_database, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~magic_load_database_file_test()
    {
        std::filesystem::remove(m_invalid_database, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "magic_load_database_file_test"
    };
    std::filesystem::path m_invalid_database{m_test_dir / "invalid_database"};
    std::filesystem::path m_nonexistent_database{
        m_test_dir / "nonexistent_database"
    };
    magic m_closed_magic{};
    magic m_opened_magic_without_database;
};

TEST_F(magic_load_database_file_test, closed_magic)
{
    EXPECT_THROW(
        m_closed_magic.load_database_file(m_valid_database),
        magic_is_closed
    );
}

TEST_F(magic_load_database_file_test, closed_magic_noexcept)
{
    EXPECT_FALSE(
        m_closed_magic.load_database_file(std::nothrow, m_valid_database)
    );
    EXPECT_FALSE(m_closed_magic.is_database_loaded());
    EXPECT_FALSE(m_closed_magic.is_valid());
    EXPECT_FALSE(m_closed_magic);
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_empty_path
)
{
    EXPECT_THROW(
        m_opened_magic_without_database.load_database_file(m_empty_path),
        empty_path
    );
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_empty_path_noexcept
)
{
    EXPECT_FALSE(m_opened_magic_without_database
                     .load_database_file(std::nothrow, m_empty_path));
    EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(m_opened_magic_without_database.is_valid());
    EXPECT_FALSE(m_opened_magic_without_database);
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_nonexistent_database
)
{
    EXPECT_THROW(
        m_opened_magic_without_database.load_database_file(
            m_nonexistent_database
        ),
        path_does_not_exist
    );
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_nonexistent_database_noexcept
)
{
    EXPECT_FALSE(m_opened_magic_without_database
                     .load_database_file(std::nothrow, m_nonexistent_database));
    EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(m_opened_magic_without_database.is_valid());
    EXPECT_FALSE(m_opened_magic_without_database);
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_directory
)
{
    EXPECT_THROW(
        m_opened_magic_without_database.load_database_file(m_test_dir),
        path_is_not_regular_file
    );
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_directory_noexcept
)
{
    EXPECT_FALSE(m_opened_magic_without_database
                     .load_database_file(std::nothrow, m_test_dir));
    EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(m_opened_magic_without_database.is_valid());
    EXPECT_FALSE(m_opened_magic_without_database);
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_invalid_database
)
{
    EXPECT_THROW(
        m_opened_magic_without_database.load_database_file(m_invalid_database),
        magic_load_database_file_error
    );
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_invalid_database_noexcept
)
{
    EXPECT_FALSE(m_opened_magic_without_database
                     .load_database_file(std::nothrow, m_invalid_database));
    EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(m_opened_magic_without_database.is_valid());
    EXPECT_FALSE(m_opened_magic_without_database);
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_valid_database
)
{
    m_opened_magic_without_database.load_database_file(m_valid_database);
    EXPECT_TRUE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_TRUE(m_opened_magic_without_database.is_valid());
    EXPECT_TRUE(m_opened_magic_without_database);
}

TEST_F(
    magic_load_database_file_test,
    opened_magic_without_database_load_valid_database_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .load_database_file(std::nothrow, m_valid_database));
    EXPECT_TRUE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_TRUE(m_opened_magic_without_database.is_valid());
    EXPECT_TRUE(m_opened_magic_without_database);
}

TEST_F(magic_load_database_file_test, load_database_files_multiple_times)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .load_database_file(std::nothrow, m_valid_database));
    EXPECT_TRUE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_TRUE(m_opened_magic_without_database.is_valid());
    EXPECT_TRUE(m_opened_magic_without_database);

    EXPECT_FALSE(m_opened_magic_without_database
                     .load_database_file(std::nothrow, m_invalid_database));
    EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_FALSE(m_opened_magic_without_database.is_valid());
    EXPECT_FALSE(m_opened_magic_without_database);

    EXPECT_TRUE(m_opened_magic_without_database
                    .load_database_file(std::nothrow, m_valid_database));
    EXPECT_TRUE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_TRUE(m_opened_magic_without_database.is_valid());
    EXPECT_TRUE(m_opened_magic_without_database);

    EXPECT_TRUE(m_opened_magic_without_database
                    .load_database_file(std::nothrow, m_valid_database));
    EXPECT_TRUE(m_opened_magic_without_database.is_database_loaded());
    EXPECT_TRUE(m_opened_magic_without_database.is_valid());
    EXPECT_TRUE(m_opened_magic_without_database);
}
