/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace recognition;
using namespace utility;

struct magic_identify_container_test : testing::Test {
protected:
    magic_identify_container_test()
    {
        EXPECT_TRUE(m_opened_magic_without_database
                        .open(magic::flags::mime, std::nothrow));
        EXPECT_FALSE(m_opened_magic_without_database.is_database_loaded());
        EXPECT_TRUE(m_valid_magic.is_valid());
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

    ~magic_identify_container_test()
    {
        std::filesystem::remove(m_binary_file, m_error_code);
        std::filesystem::remove(m_text_file, m_error_code);
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
        std::filesystem::temp_directory_path() / "magic_identify_container_test"
    };
    std::filesystem::path   m_nonexistent_path{m_test_dir / "nonexistent_path"};
    std::filesystem::path   m_text_file   = m_test_dir / "text.txt";
    std::filesystem::path   m_binary_file = m_test_dir / "binary.txt";
    magic::types_of_files_t m_types_of_valid_files{
        {m_text_file,   "text/plain; charset=us-ascii"            },
        {m_binary_file, "application/octet-stream; charset=binary"}
    };
    magic::expected_types_of_files_t m_expected_types_of_valid_files{
        {m_text_file,   "text/plain; charset=us-ascii"            },
        {m_binary_file, "application/octet-stream; charset=binary"}
    };
    std::vector<std::filesystem::path> m_empty_container{};
    std::vector<std::filesystem::path> m_empty_path_container{{}, {}};
    std::vector<std::filesystem::path> m_nonexistent_path_container{
        m_nonexistent_path,
        m_nonexistent_path
    };
    magic::expected_types_of_files_t
        m_expected_types_of_nonexistent_path_container{
            {m_nonexistent_path,
             std::unexpected{path_does_not_exist{m_nonexistent_path}.what()}}
    };
    std::vector<std::filesystem::path> m_valid_container{
        m_text_file,
        m_binary_file
    };
    magic m_closed_magic{};
    magic m_opened_magic_without_database;
    magic m_valid_magic{magic::flags::mime, std::nothrow, m_valid_database};
    magic::tracker_t m_null_tracker{nullptr};
    magic::tracker_t m_tracker{make_shared_progress_tracker()};
};

TEST_F(magic_identify_container_test, closed_magic_empty_container)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_empty_container)),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_empty_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_empty_container, m_null_tracker)
        ),
        magic_is_closed
    );
}

TEST_F(magic_identify_container_test, closed_magic_empty_container_with_tracker)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_empty_container, m_tracker)
        ),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, closed_magic_empty_container_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.identify_files(m_empty_container, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_empty_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .identify_files(m_empty_container, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_empty_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_empty_container, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, closed_magic_empty_path_container)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_empty_path_container)
        ),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_empty_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(
            m_empty_path_container,
            m_null_tracker
        )),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_empty_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_empty_path_container, m_tracker)
        ),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    closed_magic_empty_path_container_noexcept
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_empty_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    closed_magic_empty_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(
                        m_empty_path_container,
                        std::nothrow,
                        m_null_tracker
                    )
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    closed_magic_empty_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .identify_files(m_empty_path_container, std::nothrow, m_tracker)
            .empty()
    );
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, closed_magic_nonexistent_path_container)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_nonexistent_path_container)
        ),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_nonexistent_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(
            m_nonexistent_path_container,
            m_null_tracker
        )),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_nonexistent_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(
            m_nonexistent_path_container,
            m_tracker
        )),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    closed_magic_nonexistent_path_container_noexcept
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_nonexistent_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    closed_magic_nonexistent_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_null_tracker
                    )
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    closed_magic_nonexistent_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_tracker
                    )
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, closed_magic_valid_contaniner)
{
    EXPECT_THROW(
        static_cast<void>(m_closed_magic.identify_files(m_valid_container)),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_valid_contaniner_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_valid_container, m_null_tracker)
        ),
        magic_is_closed
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_valid_contaniner_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_closed_magic.identify_files(m_valid_container, m_tracker)
        ),
        magic_is_closed
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, closed_magic_valid_contaniner_noexcept)
{
    EXPECT_TRUE(
        m_closed_magic.identify_files(m_valid_container, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_valid_contaniner_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_closed_magic
            .identify_files(m_valid_container, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_container_test,
    closed_magic_valid_contaniner_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_closed_magic
                    .identify_files(m_valid_container, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_container
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_files(m_empty_container)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_empty_container,
            m_null_tracker
        )),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_empty_container, m_tracker)),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_container_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_container, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .identify_files(m_empty_container, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_container, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_path_container
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_empty_path_container
        )),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_empty_path_container,
            m_null_tracker
        )),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_empty_path_container,
            m_tracker
        )),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_path_container_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_empty_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(
                        m_empty_path_container,
                        std::nothrow,
                        m_null_tracker
                    )
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_empty_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .identify_files(m_empty_path_container, std::nothrow, m_tracker)
            .empty()
    );
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_nonexistent_path_container
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_nonexistent_path_container
        )),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_nonexistent_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_nonexistent_path_container,
            m_null_tracker
        )),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_nonexistent_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_nonexistent_path_container,
            m_tracker
        )),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_nonexistent_path_container_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_nonexistent_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_nonexistent_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_null_tracker
                    )
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_nonexistent_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_tracker
                    )
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_valid_container
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_opened_magic_without_database.identify_files(m_valid_container)
        ),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_valid_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database.identify_files(
            m_valid_container,
            m_null_tracker
        )),
        magic_database_not_loaded
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_valid_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_opened_magic_without_database
                              .identify_files(m_valid_container, m_tracker)),
        magic_database_not_loaded
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_valid_contaniner_noexcept
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_valid_container, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_valid_contaniner_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_opened_magic_without_database
            .identify_files(m_valid_container, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_container_test,
    opened_magic_without_database_valid_contaniner_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_opened_magic_without_database
                    .identify_files(m_valid_container, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, valid_magic_empty_container)
{
    EXPECT_TRUE(m_valid_magic.identify_files(m_empty_container).empty());
}

TEST_F(
    magic_identify_container_test,
    valid_magic_empty_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_empty_container, m_null_tracker)
        ),
        null_tracker
    );
}

TEST_F(magic_identify_container_test, valid_magic_empty_container_with_tracker)
{
    EXPECT_TRUE(
        m_valid_magic.identify_files(m_empty_container, m_tracker).empty()
    );
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, valid_magic_empty_container_noexcept)
{
    EXPECT_TRUE(
        m_valid_magic.identify_files(m_empty_container, std::nothrow).empty()
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_empty_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .identify_files(m_empty_container, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_empty_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_empty_container, std::nothrow, m_tracker)
                    .empty());
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, valid_magic_empty_path_container)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_files(m_empty_path_container)),
        empty_path
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_empty_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_empty_path_container, m_null_tracker)
        ),
        null_tracker
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_empty_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_empty_path_container, m_tracker)
        ),
        empty_path
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, valid_magic_empty_path_container_noexcept)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(m_empty_path_container, std::nothrow)
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    valid_magic_empty_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(
                        m_empty_path_container,
                        std::nothrow,
                        m_null_tracker
                    )
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    valid_magic_empty_path_container_noexcept_with_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .identify_files(m_empty_path_container, std::nothrow, m_tracker)
            .empty()
    );
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, valid_magic_nonexistent_path_container)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_nonexistent_path_container)
        ),
        path_does_not_exist
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_nonexistent_path_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_files(
            m_nonexistent_path_container,
            m_null_tracker
        )),
        null_tracker
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_nonexistent_path_container_with_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(m_valid_magic.identify_files(
            m_nonexistent_path_container,
            m_tracker
        )),
        path_does_not_exist
    );
    EXPECT_FALSE(m_tracker->is_completed());
}

TEST_F(
    magic_identify_container_test,
    valid_magic_nonexistent_path_container_noexcept
)
{
    EXPECT_EQ(
        m_valid_magic
            .identify_files(m_nonexistent_path_container, std::nothrow),
        m_expected_types_of_nonexistent_path_container
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_nonexistent_path_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(m_valid_magic
                    .identify_files(
                        m_nonexistent_path_container,
                        std::nothrow,
                        m_null_tracker
                    )
                    .empty());
}

TEST_F(
    magic_identify_container_test,
    valid_magic_nonexistent_path_container_noexcept_with_tracker
)
{
    EXPECT_EQ(
        m_valid_magic.identify_files(
            m_nonexistent_path_container,
            std::nothrow,
            m_tracker
        ),
        m_expected_types_of_nonexistent_path_container
    );
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, valid_magic_valid_container)
{
    EXPECT_EQ(
        m_valid_magic.identify_files(m_valid_container),
        m_types_of_valid_files
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_valid_container_with_null_tracker
)
{
    EXPECT_THROW(
        static_cast<void>(
            m_valid_magic.identify_files(m_valid_container, m_null_tracker)
        ),
        null_tracker
    );
}

TEST_F(magic_identify_container_test, valid_magic_valid_container_with_tracker)
{
    EXPECT_EQ(
        m_valid_magic.identify_files(m_valid_container, m_tracker),
        m_types_of_valid_files
    );
    EXPECT_TRUE(m_tracker->is_completed());
}

TEST_F(magic_identify_container_test, valid_magic_valid_container_noexcept)
{
    EXPECT_EQ(
        m_valid_magic.identify_files(m_valid_container, std::nothrow),
        m_expected_types_of_valid_files
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_valid_container_noexcept_with_null_tracker
)
{
    EXPECT_TRUE(
        m_valid_magic
            .identify_files(m_valid_container, std::nothrow, m_null_tracker)
            .empty()
    );
}

TEST_F(
    magic_identify_container_test,
    valid_magic_valid_container_noexcept_with_tracker
)
{
    EXPECT_EQ(
        m_valid_magic
            .identify_files(m_valid_container, std::nothrow, m_tracker),
        m_expected_types_of_valid_files
    );
    EXPECT_TRUE(m_tracker->is_completed());
}

