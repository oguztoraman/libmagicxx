/* SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <vector>

#include <gtest/gtest.h>
#include <file_concepts.hpp>

using namespace file_concepts;

TEST(magic_file_concepts_test, empty_file_container_to_string_conversion_test)
{
    EXPECT_TRUE(to_string(std::vector<std::filesystem::path>{}).empty());
}

TEST(magic_file_concepts_test, one_file_to_string_conversion_test)
{
    std::vector<std::filesystem::path> file{"/dev/null"};
    EXPECT_EQ(to_string(file), "/dev/null");
}

TEST(magic_file_concepts_test, two_files_to_string_conversion_test)
{
    std::vector<std::filesystem::path> file{"/dev/null", "/media"};
    EXPECT_EQ(to_string(file), "/dev/null, /media");
}

TEST(magic_file_concepts_test, multiple_files_to_string_conversion_test)
{
    std::vector<std::filesystem::path> files{
        "/tmp",
        "/usr",
        "/include",
        "/home",
        "/root",
    };
    EXPECT_EQ(to_string(files),
        "/tmp, "
        "/usr, "
        "/include, "
        "/home, "
        "/root"
    );
}
