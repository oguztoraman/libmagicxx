/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace recognition;

struct magic_identify_file_test : testing::Test {
protected:
    magic_identify_file_test()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .open(magic::flags::mime, std::nothrow));
        EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
        EXPECT_TRUE(m_valid_magic.is_valid());
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~magic_identify_file_test()
    {
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "magic_identify_file_test"
    };
    std::filesystem::path m_nonexistent_path{m_test_dir / "nonexistent_path"};
    magic                 m_closed_magic{};
    magic                 m_opened_magic_without_database;
    magic m_valid_magic{magic::flags::mime, std::nothrow, m_valid_database};
};

TEST_F(magic_identify_file_test, closed_magic_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_file(m_empty_path)),
        magic_is_closed
    );
}

TEST_F(magic_identify_file_test, closed_magic_empty_path_noexcept)
{
    EXPECT_EQ(
        m_closed_magic.identify_file(m_empty_path, std::nothrow).error(),
        magic_is_closed{}.what()
    );
}

TEST_F(magic_identify_file_test, closed_magic_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_file(m_nonexistent_path)),
        magic_is_closed
    );
}

TEST_F(magic_identify_file_test, closed_magic_nonexistent_path_noexcept)
{
    EXPECT_EQ(
        m_closed_magic.identify_file(m_nonexistent_path, std::nothrow).error(),
        magic_is_closed{}.what()
    );
}

TEST_F(magic_identify_file_test, closed_magic_valid_database)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_file(m_valid_database)),
        magic_is_closed
    );
}

TEST_F(magic_identify_file_test, closed_magic_valid_database_noexcept)
{
    EXPECT_EQ(
        m_closed_magic.identify_file(m_valid_database, std::nothrow).error(),
        magic_is_closed{}.what()
    );
}

TEST_F(magic_identify_file_test, opened_magic_without_database_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_file(m_empty_path)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_file_test,
    opened_magic_without_database_empty_path_noexcept
)
{
    EXPECT_EQ(
        m_opened_magic_without_database
            .identify_file(m_empty_path, std::nothrow)
            .error(),
        magic_database_not_loaded{}.what()
    );
}

TEST_F(magic_identify_file_test, opened_magic_without_database_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_file(m_nonexistent_path)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_file_test,
    opened_magic_without_database_nonexistent_path_noexcept
)
{
    EXPECT_EQ(
        m_opened_magic_without_database
            .identify_file(m_nonexistent_path, std::nothrow)
            .error(),
        magic_database_not_loaded{}.what()
    );
}

TEST_F(magic_identify_file_test, opened_magic_without_database_valid_database)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_file(m_valid_database)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_file_test,
    opened_magic_without_database_valid_database_noexcept
)
{
    EXPECT_EQ(
        m_opened_magic_without_database
            .identify_file(m_valid_database, std::nothrow)
            .error(),
        magic_database_not_loaded{}.what()
    );
}

TEST_F(magic_identify_file_test, valid_magic_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_file(m_empty_path)),
        empty_path
    );
}

TEST_F(magic_identify_file_test, valid_magic_empty_path_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.identify_file(m_empty_path, std::nothrow).error(),
        empty_path{}.what()
    );
}

TEST_F(magic_identify_file_test, valid_magic_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_file(m_nonexistent_path)),
        path_does_not_exist
    );
}

TEST_F(magic_identify_file_test, valid_magic_nonexistent_path_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.identify_file(m_nonexistent_path, std::nothrow).error(),
        path_does_not_exist{m_nonexistent_path}.what()
    );
}

TEST_F(magic_identify_file_test, valid_magic_valid_database)
{
    EXPECT_EQ(
        m_valid_magic.identify_file(m_valid_database, std::nothrow).value(),
        "text/x-file; charset=us-ascii"
    );
}

TEST_F(magic_identify_file_test, valid_magic_valid_database_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.identify_file(m_valid_database),
        "text/x-file; charset=us-ascii"
    );
}

TEST_F(magic_identify_file_test, valid_magic_test_dir)
{
    EXPECT_EQ(
        m_valid_magic.identify_file(m_test_dir),
        "inode/directory; charset=binary"
    );
}

TEST_F(magic_identify_file_test, valid_magic_test_dir_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.identify_file(m_test_dir, std::nothrow).value(),
        "inode/directory; charset=binary"
    );
}
