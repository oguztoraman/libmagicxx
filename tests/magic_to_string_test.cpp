/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace recognition;
using namespace file_concepts;

TEST(magic_to_string_test, type_of_a_file_t)
{
    EXPECT_EQ(
        to_string(magic::type_of_a_file_t{"path1", "type1"}),
        "path1 -> type1"
    );
}

TEST(magic_to_string_test, types_of_files_t)
{
    EXPECT_EQ(
        to_string(magic::types_of_files_t{
            {"path1", "type1"},
            {"path2", "type2"},
            {"path3", "type3"}
    }),
        "path1 -> type1\n"
        "path2 -> type2\n"
        "path3 -> type3"
    );
}

TEST(magic_to_string_test, expected_type_of_a_file_t)
{
    EXPECT_EQ(
        to_string(magic::expected_type_of_a_file_t{"path1", "type1"}),
        "path1 -> type1"
    );
    EXPECT_EQ(
        to_string(
            magic::expected_type_of_a_file_t{"path1", std::unexpected{"error1"}}
        ),
        "path1 -> error1"
    );
}

TEST(magic_to_string_test, expected_types_of_files_t)
{
    EXPECT_EQ(
        to_string(magic::expected_types_of_files_t{
            {"path1", "type1"                  },
            {"path2", std::unexpected{"error1"}},
            {"path3", "type2"                  }
    }),
        "path1 -> type1\n"
        "path2 -> error1\n"
        "path3 -> type2"
    );
}

TEST(magic_to_string_test, flags)
{
    using enum magic::flags;
    EXPECT_EQ(to_string(none), "none");
    EXPECT_EQ(to_string(debug), "debug");
    EXPECT_EQ(to_string(symlink), "symlink");
    EXPECT_EQ(to_string(compress), "compress");
    EXPECT_EQ(to_string(devices), "devices");
    EXPECT_EQ(to_string(mime_type), "mime_type");
    EXPECT_EQ(to_string(continue_search), "continue_search");
    EXPECT_EQ(to_string(check_database), "check_database");
    EXPECT_EQ(to_string(preserve_atime), "preserve_atime");
    EXPECT_EQ(to_string(raw), "raw");
    EXPECT_EQ(to_string(error), "error");
    EXPECT_EQ(to_string(mime_encoding), "mime_encoding");
    EXPECT_EQ(to_string(mime), "mime");
    EXPECT_EQ(to_string(apple), "apple");
    EXPECT_EQ(to_string(extension), "extension");
    EXPECT_EQ(to_string(compress_transp), "compress_transp");
    EXPECT_EQ(to_string(no_compress_fork), "no_compress_fork");
    EXPECT_EQ(to_string(nodesc), "nodesc");
    EXPECT_EQ(to_string(no_check_compress), "no_check_compress");
    EXPECT_EQ(to_string(no_check_tar), "no_check_tar");
    EXPECT_EQ(to_string(no_check_soft), "no_check_soft");
    EXPECT_EQ(to_string(no_check_apptype), "no_check_apptype");
    EXPECT_EQ(to_string(no_check_elf), "no_check_elf");
    EXPECT_EQ(to_string(no_check_text), "no_check_text");
    EXPECT_EQ(to_string(no_check_cdf), "no_check_cdf");
    EXPECT_EQ(to_string(no_check_csv), "no_check_csv");
    EXPECT_EQ(to_string(no_check_tokens), "no_check_tokens");
    EXPECT_EQ(to_string(no_check_encoding), "no_check_encoding");
    EXPECT_EQ(to_string(no_check_json), "no_check_json");
    EXPECT_EQ(to_string(no_check_simh), "no_check_simh");
    EXPECT_EQ(to_string(no_check_builtin), "no_check_builtin");
}

TEST(magic_to_string_test, flags_container_t)
{
    using enum magic::flags;
    EXPECT_EQ(
        to_string(magic::flags_container_t{
            none,
            debug,
            symlink,
            compress,
            devices,
            mime_type,
            continue_search,
            check_database,
            preserve_atime,
            raw,
            error,
            mime_encoding,
            mime,
            apple,
            extension,
            compress_transp,
            no_compress_fork,
            nodesc,
            no_check_compress,
            no_check_tar,
            no_check_soft,
            no_check_apptype,
            no_check_elf,
            no_check_text,
            no_check_cdf,
            no_check_csv,
            no_check_tokens,
            no_check_encoding,
            no_check_json,
            no_check_simh,
            no_check_builtin
        }),
        "none, "
        "debug, "
        "symlink, "
        "compress, "
        "devices, "
        "mime_type, "
        "continue_search, "
        "check_database, "
        "preserve_atime, "
        "raw, "
        "error, "
        "mime_encoding, "
        "mime, "
        "apple, "
        "extension, "
        "compress_transp, "
        "no_compress_fork, "
        "nodesc, "
        "no_check_compress, "
        "no_check_tar, "
        "no_check_soft, "
        "no_check_apptype, "
        "no_check_elf, "
        "no_check_text, "
        "no_check_cdf, "
        "no_check_csv, "
        "no_check_tokens, "
        "no_check_encoding, "
        "no_check_json, "
        "no_check_simh, "
        "no_check_builtin"
    );
}

TEST(magic_to_string_test, parameters)
{
    using enum magic::parameters;
    EXPECT_EQ(to_string(indir_max), "indir_max");
    EXPECT_EQ(to_string(name_max), "name_max");
    EXPECT_EQ(to_string(elf_phnum_max), "elf_phnum_max");
    EXPECT_EQ(to_string(elf_shnum_max), "elf_shnum_max");
    EXPECT_EQ(to_string(elf_notes_max), "elf_notes_max");
    EXPECT_EQ(to_string(regex_max), "regex_max");
    EXPECT_EQ(to_string(bytes_max), "bytes_max");
    EXPECT_EQ(to_string(encoding_max), "encoding_max");
    EXPECT_EQ(to_string(elf_shsize_max), "elf_shsize_max");
    EXPECT_EQ(to_string(mag_warn_max), "mag_warn_max");
}

TEST(magic_to_string_test, parameter_value_t)
{
    using enum magic::parameters;
    EXPECT_EQ(
        to_string(magic::parameter_value_t{indir_max, 1}),
        "indir_max: 1"
    );
}

TEST(magic_to_string_test, parameter_value_map_t)
{
    using enum magic::parameters;
    EXPECT_EQ(
        to_string(magic::parameter_value_map_t{
            {indir_max,      1 },
            {name_max,       2 },
            {elf_phnum_max,  3 },
            {elf_shnum_max,  4 },
            {elf_notes_max,  5 },
            {regex_max,      6 },
            {bytes_max,      7 },
            {encoding_max,   8 },
            {elf_shsize_max, 9 },
            {mag_warn_max,   10}
    }),
        "indir_max: 1, "
        "name_max: 2, "
        "elf_phnum_max: 3, "
        "elf_shnum_max: 4, "
        "elf_notes_max: 5, "
        "regex_max: 6, "
        "bytes_max: 7, "
        "encoding_max: 8, "
        "elf_shsize_max: 9, "
        "mag_warn_max: 10"
    );
}

TEST(magic_to_string_test, empty_file_container)
{
    EXPECT_TRUE(to_string(std::vector<std::filesystem::path>{}).empty());
}

TEST(magic_to_string_test, one_file)
{
    std::vector<std::filesystem::path> file{"/dev/null"};
    EXPECT_EQ(to_string(file), "/dev/null");
}

TEST(magic_to_string_test, two_files)
{
    std::vector<std::filesystem::path> file{"/dev/null", "/media"};
    EXPECT_EQ(to_string(file), "/dev/null, /media");
}

TEST(magic_to_string_test, multiple_files)
{
    std::vector<std::filesystem::path> files{
        "/tmp",
        "/usr",
        "/include",
        "/home",
        "/root",
    };
    EXPECT_EQ(
        to_string(files),
        "/tmp, "
        "/usr, "
        "/include, "
        "/home, "
        "/root"
    );
}
