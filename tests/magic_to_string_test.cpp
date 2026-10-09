/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_to_string_test.cpp
 * @brief Unit tests for ToString() conversions.
 *
 * Tests string conversion functions for Magic types including:
 * - FileTypeEntryT and FileTypeMapT free functions
 * - ExpectedFileTypeEntryT and ExpectedFileTypeMapT free functions
 * - MagicFlags::ToString() member function
 * - MagicParameters::ToString() member conversions
 *
 * @section to_string_test_types Types Tested
 *
 * | Type | Format |
 * |------|--------|
 * | FileTypeEntryT | "path -> type" |
 * | FileTypeMapT | Entries separated by newlines |
 * | ExpectedFileTypeEntryT | "path -> type" or "path -> error" |
 * | MagicFlags | Comma-separated names (e.g., "Compress, Mime") |
 * | MagicParameters | "name: value" entries (e.g., "BytesMax: 1048576") |
 *
 * @note Exhaustive flags and parameters string coverage lives in
 *       tests/magic_flags_test.cpp and tests/magic_parameters_test.cpp.
 *
 * @see ToString(Magic::FileTypeEntryT)
 * @see MagicFlags::ToString()
 * @see MagicParameters::ToString()
 */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace Recognition;
using namespace Utility;

TEST(MagicToStringTest, file_type_entry_t)
{
    EXPECT_EQ(
        ToString(Magic::FileTypeEntryT{"path1", "type1"}),
        "path1 -> type1"
    );
}

TEST(MagicToStringTest, file_type_map_t)
{
    EXPECT_EQ(
        ToString(
            Magic::FileTypeMapT{
                {"path1", "type1"},
                {"path2", "type2"},
                {"path3", "type3"}
    }
        ),
        "path1 -> type1\n"
        "path2 -> type2\n"
        "path3 -> type3"
    );
}

TEST(MagicToStringTest, expected_file_type_entry_t)
{
    EXPECT_EQ(
        ToString(Magic::ExpectedFileTypeEntryT{"path1", "type1"}),
        "path1 -> type1"
    );
    EXPECT_EQ(
        ToString(
            Magic::ExpectedFileTypeEntryT{"path1", std::unexpected{"error1"}}
        ),
        "path1 -> error1"
    );
}

TEST(MagicToStringTest, expected_file_type_map_t)
{
    EXPECT_EQ(
        ToString(
            Magic::ExpectedFileTypeMapT{
                {"path1", "type1"                  },
                {"path2", std::unexpected{"error1"}},
                {"path3", "type2"                  }
    }
        ),
        "path1 -> type1\n"
        "path2 -> error1\n"
        "path3 -> type2"
    );
}

TEST(MagicToStringTest, flags)
{
    EXPECT_EQ(MagicFlags{Magic::FlagsT::Mime}.ToString(), "Mime");
    EXPECT_EQ(MagicFlags{}.ToString(), "None");
    EXPECT_EQ(
        MagicFlags{Magic::FlagsT::Mime | Magic::FlagsT::Compress}.ToString(),
        "Compress, Mime"
    );
}

TEST(MagicToStringTest, flags_container_t)
{
    const Magic::FlagsContainerT container{
        Magic::FlagsT::Debug,
        Magic::FlagsT::Compress,
        Magic::FlagsT::Mime
    };
    EXPECT_EQ(MagicFlags{container}.ToString(), "Debug, Compress, Mime");
    const Magic::FlagsContainerT none_container{Magic::FlagsT::None};
    EXPECT_EQ(MagicFlags{none_container}.ToString(), "None");
    const Magic::FlagsContainerT mixed_container{
        Magic::FlagsT::None,
        Magic::FlagsT::Mime,
        Magic::FlagsT::Compress
    };
    EXPECT_EQ(MagicFlags{mixed_container}.ToString("\n"), "Compress\nMime");
}

TEST(MagicToStringTest, parameters)
{
    using enum Magic::ParametersT;
    EXPECT_EQ(
        (MagicParameters{IndirMax, 0UZ}.ToString(
            MagicParameters::StringFormat::Names,
            ": ",
            ", "
        )),
        "IndirMax"
    );
    EXPECT_EQ(
        (MagicParameters{BytesMax, 1'048'576UZ}.ToString()),
        "BytesMax: 1048576"
    );
}

TEST(MagicToStringTest, parameter_value_map_t)
{
    using enum Magic::ParametersT;
    const Magic::ParameterValueMapT all_values{
        {IndirMax,     1UZ },
        {NameMax,      2UZ },
        {ElfPhnumMax,  3UZ },
        {ElfShnumMax,  4UZ },
        {ElfNotesMax,  5UZ },
        {RegexMax,     6UZ },
        {BytesMax,     7UZ },
        {EncodingMax,  8UZ },
        {ElfShsizeMax, 9UZ },
        {MagWarnMax,   10UZ}
    };
    const MagicParameters parameters{all_values};
    EXPECT_EQ(
        parameters.ToString(),
        "IndirMax: 1, NameMax: 2, ElfPhnumMax: 3, ElfShnumMax: 4, "
        "ElfNotesMax: 5, RegexMax: 6, BytesMax: 7, EncodingMax: 8, "
        "ElfShsizeMax: 9, MagWarnMax: 10"
    );
    const Magic::ParameterValueMapT two_values{
        {BytesMax, 7UZ},
        {IndirMax, 1UZ}
    };
    const MagicParameters subset{two_values};
    EXPECT_EQ(
        subset.ToString(
            MagicParameters::StringFormat::NamesAndValues,
            "=",
            ";"
        ),
        "IndirMax=1;BytesMax=7"
    );
}

TEST(MagicToStringTest, empty_file_container)
{
    EXPECT_TRUE(ToString(std::vector<std::filesystem::path>{}).empty());
}

TEST(MagicToStringTest, one_file)
{
    std::vector<std::filesystem::path> file{"/dev/null"};
    EXPECT_EQ(ToString(file), "/dev/null");
}

TEST(MagicToStringTest, two_files)
{
    std::vector<std::filesystem::path> file{"/dev/null", "/media"};
    EXPECT_EQ(ToString(file), "/dev/null, /media");
}

TEST(MagicToStringTest, multiple_files)
{
    std::vector<std::filesystem::path> files{
        "/tmp",
        "/usr",
        "/include",
        "/home",
        "/root",
    };
    EXPECT_EQ(
        ToString(files),
        "/tmp, "
        "/usr, "
        "/include, "
        "/home, "
        "/root"
    );
}
