/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_identify_container_test.cpp
 * @brief Unit tests for Magic::IdentifyFiles() with file containers.
 *
 * Tests container-based file identification functionality where
 * a collection of file paths is passed for batch identification.
 *
 * @section identify_container_test_fixtures Test Fixture Setup
 *
 * The test fixture creates:
 * - A temporary directory
 * - Test files (text and binary)
 * - Various container types (empty, with empty paths, with valid paths)
 *
 * @section identify_container_test_cases Test Case Categories
 *
 * | Category | Description |
 * |----------|-------------|
 * | Empty container | No files to identify |
 * | Empty paths | Container with empty path entries |
 * | Non-existent paths | Container with paths that don't exist |
 * | Valid container | Container with existing, readable files |
 * | Progress tracking | Container identification with tracker |
 *
 * @see Magic::IdentifyFiles()
 * @see Utility::FileContainer concept
 */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace Recognition;
using namespace Utility;

struct MagicIdentifyContainerTest : testing::Test {
protected:
    MagicIdentifyContainerTest()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .Open(Magic::Flags::Mime, std::nothrow));
        EXPECT_FALSE(m_opened_magic_without_database.IsDatabaseLoaded());
        EXPECT_TRUE(m_valid_magic.IsValid());
        std::filesystem::create_directory(m_test_dir, m_error_code);
        EXPECT_TRUE(std::filesystem::exists(m_test_dir, m_error_code));
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

    ~MagicIdentifyContainerTest()
    {
        std::filesystem::remove(m_binary_file, m_error_code);
        std::filesystem::remove(m_text_file, m_error_code);
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
        std::filesystem::temp_directory_path() / "magic_identify_container_test"
    };
    std::filesystem::path   m_nonexistent_path{m_test_dir / "nonexistent_path"};
    std::filesystem::path   m_text_file   = m_test_dir / "text.txt";
    std::filesystem::path   m_binary_file = m_test_dir / "binary.txt";
    Magic::FileTypeMapT m_types_of_valid_files{
        {m_text_file,   "text/plain; charset=us-ascii"            },
        {m_binary_file, "application/octet-stream; charset=binary"}
    };
    Magic::ExpectedFileTypeMapT m_expected_types_of_valid_files{
        {m_text_file,   "text/plain; charset=us-ascii"            },
        {m_binary_file, "application/octet-stream; charset=binary"}
    };
    std::vector<std::filesystem::path> m_empty_container{};
    std::vector<std::filesystem::path> m_empty_path_container{{}, {}};
    std::vector<std::filesystem::path> m_nonexistent_path_container{
        m_nonexistent_path,
        m_nonexistent_path
    };
    Magic::ExpectedFileTypeMapT
        m_expected_types_of_nonexistent_path_container{
            {m_nonexistent_path,
             std::unexpected{PathDoesNotExist{m_nonexistent_path}.what()}}
    };
    std::vector<std::filesystem::path> m_valid_container{
        m_text_file,
        m_binary_file
    };
    Magic m_closed_magic{};
    Magic m_opened_magic_without_database;
    Magic m_valid_magic{Magic::Flags::Mime, std::nothrow, m_valid_database};
    Magic::ProgressTrackerT m_null_progress_tracker{nullptr};
    Magic::ProgressTrackerT m_progress_tracker{MakeSharedProgressTracker()};
};

TEST_F(MagicIdentifyContainerTest, closed_magic_empty_container)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_empty_container)),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_empty_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_empty_container, m_null_progress_tracker)
        ),
        MagicIsClosed
    );
}

TEST_F(MagicIdentifyContainerTest, closed_magic_empty_container_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_empty_container, m_progress_tracker)
        ),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, closed_magic_empty_container_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.IdentifyFiles(m_empty_container, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_empty_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .IdentifyFiles(m_empty_container, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_empty_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_empty_container, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, closed_magic_empty_path_container)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_empty_path_container)
        ),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_empty_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(
            m_empty_path_container,
            m_null_progress_tracker
        )),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_empty_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_empty_path_container, m_progress_tracker)
        ),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_empty_path_container_noexcept
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_empty_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_empty_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(
                        m_empty_path_container,
                        std::nothrow,
                        m_null_progress_tracker
                    )
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_empty_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .IdentifyFiles(m_empty_path_container, std::nothrow, m_progress_tracker)
            .empty()
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, closed_magic_nonexistent_path_container)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_nonexistent_path_container)
        ),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_nonexistent_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(
            m_nonexistent_path_container,
            m_null_progress_tracker
        )),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_nonexistent_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(
            m_nonexistent_path_container,
            m_progress_tracker
        )),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_nonexistent_path_container_noexcept
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_nonexistent_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_nonexistent_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_null_progress_tracker
                    )
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_nonexistent_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_progress_tracker
                    )
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, closed_magic_valid_contaniner)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.IdentifyFiles(m_valid_container)),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_valid_contaniner_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_valid_container, m_null_progress_tracker)
        ),
        MagicIsClosed
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_valid_contaniner_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.IdentifyFiles(m_valid_container, m_progress_tracker)
        ),
        MagicIsClosed
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, closed_magic_valid_contaniner_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.IdentifyFiles(m_valid_container, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_valid_contaniner_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .IdentifyFiles(m_valid_container, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    closed_magic_valid_contaniner_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .IdentifyFiles(m_valid_container, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_container
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFiles(m_empty_container)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_empty_container,
            m_null_progress_tracker
        )),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_empty_container, m_progress_tracker)),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_container_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_container, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .IdentifyFiles(m_empty_container, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_container, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_path_container
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_empty_path_container
        )),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_empty_path_container,
            m_null_progress_tracker
        )),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_empty_path_container,
            m_progress_tracker
        )),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_path_container_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_empty_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(
                        m_empty_path_container,
                        std::nothrow,
                        m_null_progress_tracker
                    )
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_empty_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .IdentifyFiles(m_empty_path_container, std::nothrow, m_progress_tracker)
            .empty()
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_nonexistent_path_container
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_nonexistent_path_container
        )),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_nonexistent_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_nonexistent_path_container,
            m_null_progress_tracker
        )),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_nonexistent_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_nonexistent_path_container,
            m_progress_tracker
        )),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_nonexistent_path_container_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_nonexistent_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_nonexistent_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_null_progress_tracker
                    )
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_nonexistent_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_progress_tracker
                    )
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_valid_container
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.IdentifyFiles(m_valid_container)
        ),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_valid_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.IdentifyFiles(
            m_valid_container,
            m_null_progress_tracker
        )),
        MagicDatabaseNotLoaded
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_valid_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .IdentifyFiles(m_valid_container, m_progress_tracker)),
        MagicDatabaseNotLoaded
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_valid_contaniner_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_valid_container, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_valid_contaniner_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .IdentifyFiles(m_valid_container, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    opened_magic_without_database_valid_contaniner_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .IdentifyFiles(m_valid_container, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, valid_magic_empty_container)
{
    EXPECT_TRUE(m_valid_magic.IdentifyFiles(m_empty_container).empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_empty_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_empty_container, m_null_progress_tracker)
        ),
        NullTracker
    );
}

TEST_F(MagicIdentifyContainerTest, valid_magic_empty_container_with_tracker)
{
    EXPECT_TRUE(
        m_valid_magic.IdentifyFiles(m_empty_container, m_progress_tracker).empty()
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, valid_magic_empty_container_noexcept)
{
    EXPECT_TRUE(
        m_valid_magic.IdentifyFiles(m_empty_container, std::nothrow).empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_empty_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .IdentifyFiles(m_empty_container, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_empty_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_empty_container, std::nothrow, m_progress_tracker)
                    .empty());
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, valid_magic_empty_path_container)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFiles(m_empty_path_container)),
        EmptyPath
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_empty_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_empty_path_container, m_null_progress_tracker)
        ),
        NullTracker
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_empty_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_empty_path_container, m_progress_tracker)
        ),
        EmptyPath
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, valid_magic_empty_path_container_noexcept)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(m_empty_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_empty_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(
                        m_empty_path_container,
                        std::nothrow,
                        m_null_progress_tracker
                    )
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_empty_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .IdentifyFiles(m_empty_path_container, std::nothrow, m_progress_tracker)
            .empty()
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, valid_magic_nonexistent_path_container)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_nonexistent_path_container)
        ),
        PathDoesNotExist
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_nonexistent_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFiles(
            m_nonexistent_path_container,
            m_null_progress_tracker
        )),
        NullTracker
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_nonexistent_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.IdentifyFiles(
            m_nonexistent_path_container,
            m_progress_tracker
        )),
        PathDoesNotExist
    );
    EXPECT_FALSE(m_progress_tracker->IsCompleted());
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_nonexistent_path_container_noexcept
)
{
    EXPECT_EQ(
        m_valid_magic
            .IdentifyFiles(m_nonexistent_path_container, std::nothrow),
        m_expected_types_of_nonexistent_path_container
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_nonexistent_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .IdentifyFiles(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_null_progress_tracker
                    )
                    .empty());
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_nonexistent_path_container_noexcept_with_tracker
)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFiles(
            m_nonexistent_path_container,
            std::nothrow,
            m_progress_tracker
        ),
        m_expected_types_of_nonexistent_path_container
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, valid_magic_valid_container)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFiles(m_valid_container),
        m_types_of_valid_files
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_valid_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.IdentifyFiles(m_valid_container, m_null_progress_tracker)
        ),
        NullTracker
    );
}

TEST_F(MagicIdentifyContainerTest, valid_magic_valid_container_with_tracker)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFiles(m_valid_container, m_progress_tracker),
        m_types_of_valid_files
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

TEST_F(MagicIdentifyContainerTest, valid_magic_valid_container_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.IdentifyFiles(m_valid_container, std::nothrow),
        m_expected_types_of_valid_files
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_valid_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .IdentifyFiles(m_valid_container, std::nothrow, m_null_progress_tracker)
            .empty()
    );
}

TEST_F(
    MagicIdentifyContainerTest,
    valid_magic_valid_container_noexcept_with_tracker
)
{
    EXPECT_EQ(
        m_valid_magic
            .IdentifyFiles(m_valid_container, std::nothrow, m_progress_tracker),
        m_expected_types_of_valid_files
    );
    EXPECT_TRUE(m_progress_tracker->IsCompleted());
}

