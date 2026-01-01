/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace recognition;
using namespace utility;

struct magic_identify_directory_test : testing::Test {
protected:
    magic_identify_directory_test()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .open(magic::flags::mime, std::nothrow));
        EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
        EXPECT_TRUE(m_valid_magic.is_valid());
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

    ~magic_identify_directory_test()
    {
        std::filesystem::remove(m_binary_file, m_error_code);
        std::filesystem::remove(m_text_file, m_error_code);
        std::filesystem::remove(m_empty_dir, m_error_code);
        std::filesystem::remove(m_test_dir, m_error_code);
    }

    void SetUp() override
    {
        m_tracker->reset(1);
    }

    std::error_code       m_error_code{};
    std::filesystem::path m_empty_path{};
    std::filesystem::path m_valid_database{DEFAULT_DATABASE_FILE};
    std::filesystem::path m_test_dir{
        std::filesystem::temp_directory_path() / "magic_identify_directory_test"
    };
    std::filesystem::path   m_empty_dir{m_test_dir / "empty_directory"};
    std::filesystem::path   m_nonexistent_path{m_test_dir / "nonexistent_path"};
    std::filesystem::path   m_text_file   = m_test_dir / "text.txt";
    std::filesystem::path   m_binary_file = m_test_dir / "binary.txt";
    magic::types_of_files_t m_types_of_valid_files{
        {m_text_file,   "text/plain; charset=us-ascii"            },
        {m_empty_dir,   "inode/directory; charset=binary"         },
        {m_binary_file, "application/octet-stream; charset=binary"}
    };
    magic::expected_types_of_files_t m_expected_types_of_valid_files{
        {m_text_file,   "text/plain; charset=us-ascii"            },
        {m_empty_dir,   "inode/directory; charset=binary"         },
        {m_binary_file, "application/octet-stream; charset=binary"}
    };
    magic m_closed_magic{};
    magic m_opened_magic_without_database;
    magic m_valid_magic{magic::flags::mime, std::nothrow, m_valid_database};
    magic::tracker_t m_null_tracker{nullptr};
    magic::tracker_t m_tracker{make_shared_progress_tracker()};
};

TEST_F(magic_identify_directory_test, closed_magic_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_empty_path)),
        magic_is_closed
    );
}

TEST_F(magic_identify_directory_test, closed_magic_empty_path_with_null_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_empty_path, m_null_tracker)
        ),
        magic_is_closed
    );
}

TEST_F(magic_identify_directory_test, closed_magic_empty_path_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_empty_path, m_tracker)
        ),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_empty_path_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.identify_files(m_empty_path, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_empty_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_empty_path, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_empty_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_empty_path, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_nonexistent_path)),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_nonexistent_path_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_nonexistent_path, m_null_tracker)
        ),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_nonexistent_path_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_nonexistent_path, m_tracker)
        ),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_nonexistent_path_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.identify_files(m_nonexistent_path, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_nonexistent_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .identify_files(m_nonexistent_path, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_nonexistent_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_nonexistent_path, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_file)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_valid_database)),
        magic_is_closed
    );
}

TEST_F(magic_identify_directory_test, closed_magic_file_with_null_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_valid_database, m_null_tracker)
        ),
        magic_is_closed
    );
}

TEST_F(magic_identify_directory_test, closed_magic_file_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_valid_database, m_tracker)
        ),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_file_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.identify_files(m_valid_database, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_file_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .identify_files(m_valid_database, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(magic_identify_directory_test, closed_magic_file_noexcept_with_tracker)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_valid_database, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_empty_directory)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_empty_dir)),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_empty_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_empty_dir, m_null_tracker)
        ),
        magic_is_closed
    );
}

TEST_F(magic_identify_directory_test, closed_magic_empty_directory_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_empty_dir, m_tracker)
        ),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_empty_directory_noexcept)
{
    EXPECT_TRUE(m_closed_magic.identify_files(m_empty_dir, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_empty_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_empty_dir, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_empty_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_empty_dir, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_valid_directory)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_test_dir)),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_valid_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_test_dir, m_null_tracker)
        ),
        magic_is_closed
    );
}

TEST_F(magic_identify_directory_test, closed_magic_valid_directory_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_test_dir, m_tracker)),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, closed_magic_valid_directory_noexcept)
{
    EXPECT_TRUE(m_closed_magic.identify_files(m_test_dir, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_valid_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_test_dir, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    closed_magic_valid_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_test_dir, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, opened_magic_without_database_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_files(m_empty_path)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_path_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_empty_path, m_null_tracker)),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_path_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_empty_path, m_tracker)),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_path_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_path, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_path, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_path, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_nonexistent_path
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_files(m_nonexistent_path)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_nonexistent_path_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_nonexistent_path,
            m_null_tracker
        )),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_nonexistent_path_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_nonexistent_path, m_tracker)),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_nonexistent_path_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_nonexistent_path, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_nonexistent_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .identify_files(m_nonexistent_path, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_nonexistent_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_nonexistent_path, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, opened_magic_without_database_file)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_files(m_valid_database)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_file_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_valid_database,
            m_null_tracker
        )),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_file_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_valid_database, m_tracker)),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_file_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_valid_database, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_file_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .identify_files(m_valid_database, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_file_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_valid_database, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_directory
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_files(m_empty_dir)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_empty_dir, m_null_tracker)),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_directory_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_empty_dir, m_tracker)),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_directory_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_dir, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_dir, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_empty_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_dir, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_valid_directory
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_files(m_test_dir)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_valid_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_test_dir, m_null_tracker)),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_valid_directory_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_test_dir, m_tracker)),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_valid_directory_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_test_dir, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_valid_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_test_dir, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    opened_magic_without_database_valid_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_test_dir, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_empty_path)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_files(m_empty_path)),
        empty_path
    );
}

TEST_F(magic_identify_directory_test, valid_magic_empty_path_with_null_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_empty_path, m_null_tracker)
        ),
        empty_path
    );
}

TEST_F(magic_identify_directory_test, valid_magic_empty_path_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_files(m_empty_path, m_tracker)
        ),
        empty_path
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_empty_path_noexcept)
{
    EXPECT_TRUE(m_valid_magic.identify_files(m_empty_path, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_empty_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_empty_path, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_empty_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_empty_path, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_nonexistent_path)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_files(m_nonexistent_path)),
        path_does_not_exist
    );
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_nonexistent_path_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_nonexistent_path, m_null_tracker)
        ),
        path_does_not_exist
    );
}

TEST_F(magic_identify_directory_test, valid_magic_nonexistent_path_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_nonexistent_path, m_tracker)
        ),
        path_does_not_exist
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_nonexistent_path_noexcept)
{
    EXPECT_TRUE(
        m_valid_magic.identify_files(m_nonexistent_path, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_nonexistent_path_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .identify_files(m_nonexistent_path, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_nonexistent_path_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_nonexistent_path, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_file)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_files(m_valid_database)),
        path_is_not_directory
    );
}

TEST_F(magic_identify_directory_test, valid_magic_file_with_null_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_valid_database, m_null_tracker)
        ),
        path_is_not_directory
    );
}

TEST_F(magic_identify_directory_test, valid_magic_file_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_valid_database, m_tracker)
        ),
        path_is_not_directory
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_file_noexcept)
{
    EXPECT_TRUE(
        m_valid_magic.identify_files(m_valid_database, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_file_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .identify_files(m_valid_database, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(magic_identify_directory_test, valid_magic_file_noexcept_with_tracker)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_valid_database, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_empty_directory)
{
    EXPECT_TRUE(m_valid_magic.identify_files(m_empty_dir).empty());
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_empty_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_empty_dir, m_null_tracker)
        ),
        null_tracker
    );
}

TEST_F(magic_identify_directory_test, valid_magic_empty_directory_with_tracker)
{
    EXPECT_TRUE(m_valid_magic.identify_files(m_empty_dir, m_tracker).empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_empty_directory_noexcept)
{
    EXPECT_TRUE(m_valid_magic.identify_files(m_empty_dir, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_empty_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_empty_dir, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_empty_directory_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_empty_dir, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_valid_directory)
{
    EXPECT_EQ(m_valid_magic.identify_files(m_test_dir), m_types_of_valid_files);
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_valid_directory_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_test_dir, m_null_tracker)
        ),
        null_tracker
    );
}

TEST_F(magic_identify_directory_test, valid_magic_valid_directory_with_tracker)
{
    EXPECT_EQ(
        m_valid_magic.identify_files(m_test_dir, m_tracker),
        m_types_of_valid_files
    );
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_directory_test, valid_magic_valid_directory_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.identify_files(m_test_dir, std::nothrow),
        m_expected_types_of_valid_files
    );
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_valid_directory_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_test_dir, std::nothrow, m_null_tracker)
                    .empty());
}

TEST_F(
    magic_identify_directory_test,
    valid_magic_valid_directory_noexcept_with_tracker
)
{
    EXPECT_EQ(
        m_valid_magic.identify_files(m_test_dir, std::nothrow, m_tracker),
        m_expected_types_of_valid_files
    );
    EXPECT_TRUE(m_tracker->is_completed());
}
