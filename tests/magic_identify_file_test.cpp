/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_identify_file_test.cpp
 * @brief Unit tests for Magic::IdentifyFile() method.
 *
 * Tests single file identification functionality including:
 * - Throwing and noexcept overloads
 * - Various Magic states (closed, opened without database, valid)
 * - Error conditions (empty path, non-existent path, etc.)
 *
 * @section identify_file_test_states Magic States Tested
 *
 * | State | Description |
 * |-------|-------------|
 * | Closed | Default-constructed, not opened |
 * | Opened without database | Open() called but not LoadDatabaseFile() |
 * | Valid | Both Open() and LoadDatabaseFile() successful |
 *
 * @see Magic::IdentifyFile()
 * @see Magic::ExpectedFileTypeT
 */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace Recognition;

struct MagicIdentifyFileTest : testing::Test {
protected:
    MagicIdentifyFileTest()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .Open(Magic::Flags::Mime, std::nothrow));
        EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
        EXPECT_TRUE(m_valid_magic.IsValid());
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
    }

    ~MagicIdentifyFileTest()
    {
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "magic_identify_file_test"
    };
    std::filesystem::path m_nonexistent_path{m_test_dir / "nonexistent_path"};
    Magic                 m_closed_magic{};
    Magic                 m_opened_magic_without_database;
    Magic m_valid_magic{Magic::Flags::Mime, std::nothrow, m_valid_database};
};

TEST_F(MagicIdentifyFileTest, closed_magic_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFile(m_empty_path)),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyFileTest, closed_magic_empty_path_noexcept)
{
    EXPECT_EQ(
        m_closed_magic.IdentifyFile(m_empty_path, std::nothrow).error(),
        MagicIsClosed{}.what()
    );
}

TEST_F(MagicIdentifyFileTest, closed_magic_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFile(m_nonexistent_path)),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyFileTest, closed_magic_nonexistent_path_noexcept)
{
    EXPECT_EQ(
        m_closed_magic.IdentifyFile(m_nonexistent_path, std::nothrow).error(),
        MagicIsClosed{}.what()
    );
}

TEST_F(MagicIdentifyFileTest, closed_magic_valid_database)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFile(m_valid_database)),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyFileTest, closed_magic_valid_database_noexcept)
{
    EXPECT_EQ(
        m_closed_magic.IdentifyFile(m_valid_database, std::nothrow).error(),
        MagicIsClosed{}.what()
    );
}

TEST_F(MagicIdentifyFileTest, opened_magic_without_database_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFile(m_empty_path)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyFileTest,
    opened_magic_without_database_empty_path_noexcept
)
{
    EXPECT_EQ(
        m_opened_magic_without_database
            .IdentifyFile(m_empty_path, std::nothrow)
            .error(),
        MagicDatabaseNotLoaded{}.what()
    );
}

TEST_F(MagicIdentifyFileTest, opened_magic_without_database_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFile(m_nonexistent_path)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyFileTest,
    opened_magic_without_database_nonexistent_path_noexcept
)
{
    EXPECT_EQ(
        m_opened_magic_without_database
            .IdentifyFile(m_nonexistent_path, std::nothrow)
            .error(),
        MagicDatabaseNotLoaded{}.what()
    );
}

TEST_F(MagicIdentifyFileTest, opened_magic_without_database_valid_database)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFile(m_valid_database)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyFileTest,
    opened_magic_without_database_valid_database_noexcept
)
{
    EXPECT_EQ(
        m_opened_magic_without_database
            .IdentifyFile(m_valid_database, std::nothrow)
            .error(),
        MagicDatabaseNotLoaded{}.what()
    );
}

TEST_F(MagicIdentifyFileTest, valid_magic_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFile(m_empty_path)),
        EmptyPath
    );
}

TEST_F(MagicIdentifyFileTest, valid_magic_empty_path_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFile(m_empty_path, std::nothrow).error(),
        EmptyPath{}.what()
    );
}

TEST_F(MagicIdentifyFileTest, valid_magic_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFile(m_nonexistent_path)),
        PathDoesNotExist
    );
}

TEST_F(MagicIdentifyFileTest, valid_magic_nonexistent_path_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFile(m_nonexistent_path, std::nothrow).error(),
        PathDoesNotExist{m_nonexistent_path}.what()
    );
}

TEST_F(MagicIdentifyFileTest, valid_magic_valid_database)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFile(m_valid_database, std::nothrow).value(),
        "text/x-file; charset=us-ascii"
    );
}

TEST_F(MagicIdentifyFileTest, valid_magic_valid_database_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFile(m_valid_database),
        "text/x-file; charset=us-ascii"
    );
}

TEST_F(MagicIdentifyFileTest, valid_magic_test_dir)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFile(m_test_dir),
        "inode/directory; charset=binary"
    );
}

TEST_F(MagicIdentifyFileTest, valid_magic_test_dir_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFile(m_test_dir, std::nothrow).value(),
        "inode/directory; charset=binary"
    );
}
