/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

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
        ToString(Magic::FileTypeMapT{
            {"path1", "type1"},
            {"path2", "type2"},
            {"path3", "type3"}
    }),
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
        ToString(Magic::ExpectedFileTypeMapT{
            {"path1", "type1"                  },
            {"path2", std::unexpected{"error1"}},
            {"path3", "type2"                  }
    }),
        "path1 -> type1\n"
        "path2 -> error1\n"
        "path3 -> type2"
    );
}

TEST(MagicToStringTest, flags)
{
    using enum Magic::Flags;
    EXPECT_EQ(ToString(None), "None");
    EXPECT_EQ(ToString(Debug), "Debug");
    EXPECT_EQ(ToString(Symlink), "Symlink");
    EXPECT_EQ(ToString(Compress), "Compress");
    EXPECT_EQ(ToString(Devices), "Devices");
    EXPECT_EQ(ToString(MimeType), "MimeType");
    EXPECT_EQ(ToString(ContinueSearch), "ContinueSearch");
    EXPECT_EQ(ToString(CheckDatabase), "CheckDatabase");
    EXPECT_EQ(ToString(PreserveAtime), "PreserveAtime");
    EXPECT_EQ(ToString(Raw), "Raw");
    EXPECT_EQ(ToString(Error), "Error");
    EXPECT_EQ(ToString(MimeEncoding), "MimeEncoding");
    EXPECT_EQ(ToString(Mime), "Mime");
    EXPECT_EQ(ToString(Apple), "Apple");
    EXPECT_EQ(ToString(Extension), "Extension");
    EXPECT_EQ(ToString(CompressTransp), "CompressTransp");
    EXPECT_EQ(ToString(NoCompressFork), "NoCompressFork");
    EXPECT_EQ(ToString(Nodesc), "Nodesc");
    EXPECT_EQ(ToString(NoCheckCompress), "NoCheckCompress");
    EXPECT_EQ(ToString(NoCheckTar), "NoCheckTar");
    EXPECT_EQ(ToString(NoCheckSoft), "NoCheckSoft");
    EXPECT_EQ(ToString(NoCheckApptype), "NoCheckApptype");
    EXPECT_EQ(ToString(NoCheckElf), "NoCheckElf");
    EXPECT_EQ(ToString(NoCheckText), "NoCheckText");
    EXPECT_EQ(ToString(NoCheckCdf), "NoCheckCdf");
    EXPECT_EQ(ToString(NoCheckCsv), "NoCheckCsv");
    EXPECT_EQ(ToString(NoCheckTokens), "NoCheckTokens");
    EXPECT_EQ(ToString(NoCheckEncoding), "NoCheckEncoding");
    EXPECT_EQ(ToString(NoCheckJson), "NoCheckJson");
    EXPECT_EQ(ToString(NoCheckSimh), "NoCheckSimh");
    EXPECT_EQ(ToString(NoCheckBuiltin), "NoCheckBuiltin");
}

TEST(MagicToStringTest, flags_container_t)
{
    using enum Magic::Flags;
    EXPECT_EQ(
        ToString(Magic::FlagsContainerT{
            None,
            Debug,
            Symlink,
            Compress,
            Devices,
            MimeType,
            ContinueSearch,
            CheckDatabase,
            PreserveAtime,
            Raw,
            Error,
            MimeEncoding,
            Mime,
            Apple,
            Extension,
            CompressTransp,
            NoCompressFork,
            Nodesc,
            NoCheckCompress,
            NoCheckTar,
            NoCheckSoft,
            NoCheckApptype,
            NoCheckElf,
            NoCheckText,
            NoCheckCdf,
            NoCheckCsv,
            NoCheckTokens,
            NoCheckEncoding,
            NoCheckJson,
            NoCheckSimh,
            NoCheckBuiltin
        }),
        "None, "
        "Debug, "
        "Symlink, "
        "Compress, "
        "Devices, "
        "MimeType, "
        "ContinueSearch, "
        "CheckDatabase, "
        "PreserveAtime, "
        "Raw, "
        "Error, "
        "MimeEncoding, "
        "Mime, "
        "Apple, "
        "Extension, "
        "CompressTransp, "
        "NoCompressFork, "
        "Nodesc, "
        "NoCheckCompress, "
        "NoCheckTar, "
        "NoCheckSoft, "
        "NoCheckApptype, "
        "NoCheckElf, "
        "NoCheckText, "
        "NoCheckCdf, "
        "NoCheckCsv, "
        "NoCheckTokens, "
        "NoCheckEncoding, "
        "NoCheckJson, "
        "NoCheckSimh, "
        "NoCheckBuiltin"
    );
}

TEST(MagicToStringTest, parameters)
{
    using enum Magic::Parameters;
    EXPECT_EQ(ToString(IndirMax), "IndirMax");
    EXPECT_EQ(ToString(NameMax), "NameMax");
    EXPECT_EQ(ToString(ElfPhnumMax), "ElfPhnumMax");
    EXPECT_EQ(ToString(ElfShnumMax), "ElfShnumMax");
    EXPECT_EQ(ToString(ElfNotesMax), "ElfNotesMax");
    EXPECT_EQ(ToString(RegexMax), "RegexMax");
    EXPECT_EQ(ToString(BytesMax), "BytesMax");
    EXPECT_EQ(ToString(EncodingMax), "EncodingMax");
    EXPECT_EQ(ToString(ElfShsizeMax), "ElfShsizeMax");
    EXPECT_EQ(ToString(MagWarnMax), "MagWarnMax");
}

TEST(MagicToStringTest, parameter_value_t)
{
    using enum Magic::Parameters;
    EXPECT_EQ(
        ToString(Magic::ParameterValueT{IndirMax, 1}),
        "IndirMax: 1"
    );
}

TEST(MagicToStringTest, parameter_value_map_t)
{
    using enum Magic::Parameters;
    EXPECT_EQ(
        ToString(Magic::ParameterValueMapT{
            {IndirMax,      1 },
            {NameMax,       2 },
            {ElfPhnumMax,   3 },
            {ElfShnumMax,   4 },
            {ElfNotesMax,   5 },
            {RegexMax,      6 },
            {BytesMax,      7 },
            {EncodingMax,   8 },
            {ElfShsizeMax,  9 },
            {MagWarnMax,    10}
    }),
        "IndirMax: 1, "
        "NameMax: 2, "
        "ElfPhnumMax: 3, "
        "ElfShnumMax: 4, "
        "ElfNotesMax: 5, "
        "RegexMax: 6, "
        "BytesMax: 7, "
        "EncodingMax: 8, "
        "ElfShsizeMax: 9, "
        "MagWarnMax: 10"
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
