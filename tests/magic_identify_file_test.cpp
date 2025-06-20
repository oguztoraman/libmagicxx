/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace recognition;

TEST(magic_identify_file_test, closed_magic_identify_file)
{
    magic m;
    auto  expected_file_type = m.identify_file(
        magic::default_database_file,
        std::nothrow
    );
    EXPECT_FALSE(m);
    EXPECT_FALSE(m.is_valid());
    EXPECT_FALSE(expected_file_type.has_value());
    EXPECT_EQ(expected_file_type.error(), "magic is closed.");
    EXPECT_THROW(
        [[maybe_unused]] auto _ = m.identify_file(magic::default_database_file),
        magic_is_closed
    );
}

TEST(magic_identify_file_test, opened_magic_identify_empty_path)
{
    magic m{magic::flags::mime, DEFAULT_DATABASE_FILE};
    EXPECT_TRUE(m);
    EXPECT_TRUE(m.is_valid());
    auto expected_file_type = m.identify_file({}, std::nothrow);
    EXPECT_FALSE(expected_file_type.has_value());
    EXPECT_EQ(expected_file_type.error(), "path is empty.");
    EXPECT_THROW([[maybe_unused]] auto _ = m.identify_file({}), empty_path);
}

TEST(magic_identify_file_test, opened_magic_database_not_loaded)
{
    magic m;
    m.open(magic::flags::mime);
    EXPECT_FALSE(m);
    EXPECT_FALSE(m.is_valid());
    auto expected_file_type = m.identify_file(
        DEFAULT_DATABASE_FILE,
        std::nothrow
    );
    EXPECT_FALSE(expected_file_type.has_value());
    EXPECT_EQ(expected_file_type.error(), "magic database is not loaded.");
    EXPECT_THROW(
        [[maybe_unused]] auto _ = m.identify_file(DEFAULT_DATABASE_FILE),
        magic_database_not_loaded
    );
}

TEST(magic_identify_file_test, opened_magic_identify_default_database)
{
    magic m{magic::flags::mime, DEFAULT_DATABASE_FILE};
    EXPECT_TRUE(m);
    EXPECT_TRUE(m.is_valid());
    EXPECT_EQ(
        m.identify_file(DEFAULT_DATABASE_FILE),
        "text/x-file; charset=us-ascii"
    );
    EXPECT_EQ(
        m.identify_file(DEFAULT_DATABASE_FILE, std::nothrow).value(),
        "text/x-file; charset=us-ascii"
    );
}
