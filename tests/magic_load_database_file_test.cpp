/* SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <fstream>

#include "magic.hpp"

using namespace recognition;

TEST(magic_load_database_file_test, closed_magic_load_default_database)
{
    EXPECT_THROW(magic{}.load_database_file(), magic_is_closed);
}

TEST(magic_load_database_file_test, opened_magic_load_empty_path)
{
    magic m;
    m.open(magic::flags::mime);
    EXPECT_THROW(m.load_database_file({}), empty_path);
}

TEST(magic_load_database_file_test, opened_magic_load_directory)
{
    constexpr auto test_path = "/tmp/magicxx_test/";
    std::filesystem::create_directory(test_path);
    magic m;
    m.open(magic::flags::mime);
    EXPECT_THROW(m.load_database_file(test_path), invalid_path);
}

TEST(magic_load_database_file_test, opened_magic_load_invalid_database)
{
    constexpr auto test_path = "/tmp/magicxx_test/";
    std::filesystem::create_directory(test_path);
    constexpr auto test_database = "/tmp/magicxx_test/invalid_database";
    std::ofstream  file{test_database, std::ios::trunc};
    file << "test";
    file.flush();
    magic m;
    m.open(magic::flags::mime);
    EXPECT_THROW(m.load_database_file(test_database), magic_load_error);
}

TEST(magic_load_database_file_test, opened_magic_load_default_database)
{
    magic m;
    m.open(magic::flags::mime);
    m.load_database_file();
}
