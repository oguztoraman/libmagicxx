/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_identify_directory_test.cpp
 * @brief Unit tests for Magic::IdentifyFiles() directory scanning.
 *
 * Tests recursive directory scanning functionality including:
 * - Throwing and noexcept overloads
 * - Progress tracker integration (null, valid trackers)
 * - Various Magic states (closed, opened without database, valid)
 * - Error conditions (empty path, non-existent directory, etc.)
 *
 * @section identify_dir_test_fixtures Test Fixture Setup
 *
 * The test fixture creates a temporary directory structure with:
 * - An empty subdirectory
 * - A text file (ASCII content)
 * - A binary file (octet-stream content)
 *
 * @section identify_dir_test_variants API Variants Tested
 *
 * | Variant | Description |
 * |---------|-------------|
 * | Basic | IdentifyFiles(directory) |
 * | With tracker | IdentifyFiles(directory, tracker) |
 * | Noexcept | IdentifyFiles(directory, nothrow) |
 * | Noexcept with tracker | IdentifyFiles(directory, nothrow, tracker) |
 *
 * @see Magic::IdentifyFiles()
 * @see Magic::FileTypeMapT
 * @see Utility::ProgressTracker
 */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace Recognition;
using namespace Utility;

struct MagicIdentifyDirectoryTest : testing::Test {
protected:
    MagicIdentifyDirectoryTest()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .Open(Magic::Flags::Mime, std::nothrow));
        EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
        EXPECT_TRUE(m_valid_magic.IsValid());
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
        std::filesystem::create_directory(m_empty_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_empty_dir, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, m_error_code));
        {
            std::ofstream text_file{m_text_file};
            text_file << "text";
            std::ofstream binary_file{m_binary_file, std::ios::binary};
            binary_file << 1;
        }
        EXPECT_TRUE(std::filesystem::exists(m_text_file, m_error_code));
        EXPECT_TRUE(std::filesystem::exists(m_binary_file, m_error_code));
    }

    ~MagicIdentifyDirectoryTest()
    {
        std::filesystem::remove(m_binary_file, m_error_code);
        std::filesystem::remove(m_text_file, m_error_code);
        std::filesystem::remove(m_empty_dir, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    void SetUp() override
    {
        m_progress_tracker->Reset(1);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "magic_identify_directory_test"
    };
    std::filesystem::path   m_empty_dir{m_test_dir / "empty_directory"};
    std::filesystem::path   m_nonexistent_path{m_test_dir / "nonexistent_path"};
    std::filesystem::path   m_text_file   = m_test_dir / "text.txt";
    std::filesystem::path   m_binary_file = m_test_dir / "binary.txt";
    Magic::FileTypeMapT m_types_of_valid_files{
        {m_text_file,   "text/plain; charset=us-ascii"            },
        {m_empty_dir,   "inode/directory; charset=binary"         },
        {m_binary_file, "application/octet-stream; charset=binary"}
    };
    Magic::ExpectedFileTypeMapT m_expected_types_of_valid_files{
        {m_text_file,   "text/plain; charset=us-ascii"            },
        {m_empty_dir,   "inode/directory; charset=binary"         },
        {m_binary_file, "application/octet-stream; charset=binary"}
    };
    Magic m_closed_magic{};
    Magic m_opened_magic_without_database;
    Magic m_valid_magic{Magic::Flags::Mime, std::nothrow, m_valid_database};
    Magic::ProgressTrackerT m_null_progress_tracker{nullptr};
    Magic::ProgressTrackerT m_progress_tracker{MakeSharedProgressTracker()};
};

TEST_F(MagicIdentifyDirectoryTest, closed_magic_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_empty_path)),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_empty_path_with_null_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_empty_path, m_null_progress_tracker)
        ),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_empty_path_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_empty_path, m_progress_tracker)
        ),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_empty_path_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.IdentifyFiles(m_empty_path, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_empty_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_empty_path, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_empty_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_empty_path, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_nonexistent_path)),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_nonexistent_path_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_nonexistent_path, m_null_progress_tracker)
        ),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_nonexistent_path_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_nonexistent_path, m_progress_tracker)
        ),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_nonexistent_path_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.IdentifyFiles(m_nonexistent_path, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_nonexistent_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .IdentifyFiles(m_nonexistent_path, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_nonexistent_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_nonexistent_path, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_file)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_valid_database)),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_file_with_null_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_valid_database, m_null_progress_tracker)
        ),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_file_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_valid_database, m_progress_tracker)
        ),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_file_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.IdentifyFiles(m_valid_database, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_file_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .IdentifyFiles(m_valid_database, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_file_noexcept_with_tracker)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_valid_database, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_empty_directory)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_empty_dir)),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_empty_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_empty_dir, m_null_progress_tracker)
        ),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_empty_directory_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_empty_dir, m_progress_tracker)
        ),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_empty_directory_noexcept)
{
    EXPECT_TRUE(m_closed_magic.IdentifyFiles(m_empty_dir, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_empty_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_empty_dir, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_empty_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_empty_dir, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_valid_directory)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_test_dir)),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_valid_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_test_dir, m_null_progress_tracker)
        ),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_valid_directory_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_test_dir, m_progress_tracker)),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, closed_magic_valid_directory_noexcept)
{
    EXPECT_TRUE(m_closed_magic.IdentifyFiles(m_test_dir, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_valid_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_test_dir, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    closed_magic_valid_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_test_dir, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, opened_magic_without_database_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFiles(m_empty_path)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_path_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_empty_path, m_null_progress_tracker)),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_path_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_empty_path, m_progress_tracker)),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_path_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_path, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_path, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_path, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_nonexistent_path
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFiles(m_nonexistent_path)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_nonexistent_path_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_nonexistent_path,
            m_null_progress_tracker
        )),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_nonexistent_path_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_nonexistent_path, m_progress_tracker)),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_nonexistent_path_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_nonexistent_path, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_nonexistent_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .IdentifyFiles(m_nonexistent_path, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_nonexistent_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_nonexistent_path, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, opened_magic_without_database_file)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFiles(m_valid_database)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_file_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_valid_database,
            m_null_progress_tracker
        )),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_file_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_valid_database, m_progress_tracker)),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_file_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_valid_database, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_file_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .IdentifyFiles(m_valid_database, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_file_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_valid_database, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_directory
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFiles(m_empty_dir)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_empty_dir, m_null_progress_tracker)),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_directory_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_empty_dir, m_progress_tracker)),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_directory_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_dir, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_dir, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_empty_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_dir, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_valid_directory
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFiles(m_test_dir)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_valid_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_test_dir, m_null_progress_tracker)),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_valid_directory_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_test_dir, m_progress_tracker)),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_valid_directory_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_test_dir, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_valid_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_test_dir, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    opened_magic_without_database_valid_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_test_dir, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFiles(m_empty_path)),
        EmptyPath
    );
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_empty_path_with_null_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_empty_path, m_null_progress_tracker)
        ),
        EmptyPath
    );
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_empty_path_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFiles(m_empty_path, m_progress_tracker)
        ),
        EmptyPath
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_empty_path_noexcept)
{
    EXPECT_TRUE(m_valid_magic.IdentifyFiles(m_empty_path, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_empty_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_empty_path, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_empty_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_empty_path, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFiles(m_nonexistent_path)),
        PathDoesNotExist
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_nonexistent_path_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_nonexistent_path, m_null_progress_tracker)
        ),
        PathDoesNotExist
    );
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_nonexistent_path_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_nonexistent_path, m_progress_tracker)
        ),
        PathDoesNotExist
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_nonexistent_path_noexcept)
{
    EXPECT_TRUE(
        m_valid_magic.IdentifyFiles(m_nonexistent_path, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_nonexistent_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .IdentifyFiles(m_nonexistent_path, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_nonexistent_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_nonexistent_path, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_file)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFiles(m_valid_database)),
        PathIsNotDirectory
    );
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_file_with_null_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_valid_database, m_null_progress_tracker)
        ),
        PathIsNotDirectory
    );
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_file_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_valid_database, m_progress_tracker)
        ),
        PathIsNotDirectory
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_file_noexcept)
{
    EXPECT_TRUE(
        m_valid_magic.IdentifyFiles(m_valid_database, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_file_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .IdentifyFiles(m_valid_database, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_file_noexcept_with_tracker)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_valid_database, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_empty_directory)
{
    EXPECT_TRUE(m_valid_magic.IdentifyFiles(m_empty_dir).empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_empty_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_empty_dir, m_null_progress_tracker)
        ),
        NullTracker
    );
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_empty_directory_with_tracker)
{
    EXPECT_TRUE(m_valid_magic.IdentifyFiles(m_empty_dir, m_progress_tracker).empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_empty_directory_noexcept)
{
    EXPECT_TRUE(m_valid_magic.IdentifyFiles(m_empty_dir, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_empty_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_empty_dir, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_empty_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_empty_dir, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_valid_directory)
{
    EXPECT_EQ(m_valid_magic.IdentifyFiles(m_test_dir), m_types_of_valid_files);
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_valid_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_test_dir, m_null_progress_tracker)
        ),
        NullTracker
    );
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_valid_directory_with_tracker)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFiles(m_test_dir, m_progress_tracker),
        m_types_of_valid_files
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyDirectoryTest, valid_magic_valid_directory_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFiles(m_test_dir, std::nothrow),
        m_expected_types_of_valid_files
    );
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_valid_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_test_dir, std::nothrow, m_null_progress_tracker)
                    .empty());
}

TEST_F(
    MagicIdentifyDirectoryTest,
    valid_magic_valid_directory_noexcept_with_tracker
)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFiles(m_test_dir, std::nothrow, m_progress_tracker),
        m_expected_types_of_valid_files
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}
