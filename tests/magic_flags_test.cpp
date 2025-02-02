/* SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include <random>

#include "magic.hpp"

using namespace recognition;

TEST(magic_flags_test, closed_magic_set_flags_and_get_flags)
{
    magic m;
    EXPECT_THROW(m.set_flags(magic::flags::mime), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_flags(), magic_is_closed);
}

TEST(magic_flags_test, opened_magic_set_flags_and_get_flags)
{
    magic m;
    m.open(magic::flags::none);
    std::mt19937                               eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist{
        0,
        magic::flags_mask_t{}.size() - 1
    };
    std::vector test_flags{
        1ULL << dist(eng),
        1ULL << dist(eng),
        1ULL << dist(eng),
        1ULL << dist(eng),
        1ULL << dist(eng),
        1ULL << dist(eng),
        1ULL << dist(eng)
    };
    std::ranges::sort(test_flags);
    magic::flags_container_t expected_magic_flags;
    std::ranges::transform(
        test_flags,
        std::back_inserter(expected_magic_flags),
        [](unsigned long long value) {
            return static_cast<magic::flags>(value);
        }
    );
    expected_magic_flags.erase(
        std::unique(expected_magic_flags.begin(), expected_magic_flags.end()),
        expected_magic_flags.end()
    );
    m.set_flags(std::ranges::fold_left(
        test_flags.begin(),
        test_flags.end(),
        test_flags.front(),
        std::bit_or<decltype(1ULL)>{}
    ));
    EXPECT_EQ(expected_magic_flags, m.get_flags());
    m.open(magic::flags::none);
    m.set_flags(expected_magic_flags);
    EXPECT_EQ(expected_magic_flags, m.get_flags());
}

TEST(magic_flags_test, magic_flags_to_string_conversion)
{
    EXPECT_EQ(
        to_string(magic::flags_container_t{
            magic::flags::none,
            magic::flags::debug,
            magic::flags::symlink,
            magic::flags::compress,
            magic::flags::devices,
            magic::flags::mime_type,
            magic::flags::continue_search,
            magic::flags::check_database,
            magic::flags::preserve_atime,
            magic::flags::raw,
            magic::flags::error,
            magic::flags::mime_encoding,
            magic::flags::mime,
            magic::flags::apple,
            magic::flags::extension,
            magic::flags::compress_transp,
            magic::flags::no_compress_fork,
            magic::flags::nodesc,
            magic::flags::no_check_compress,
            magic::flags::no_check_tar,
            magic::flags::no_check_soft,
            magic::flags::no_check_apptype,
            magic::flags::no_check_elf,
            magic::flags::no_check_text,
            magic::flags::no_check_cdf,
            magic::flags::no_check_csv,
            magic::flags::no_check_tokens,
            magic::flags::no_check_encoding,
            magic::flags::no_check_json,
            magic::flags::no_check_simh,
            magic::flags::no_check_builtin
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
