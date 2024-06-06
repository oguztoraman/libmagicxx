/* SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <random>

#include <magic.hpp>
#include <gtest/gtest.h>

using namespace recognition;

TEST(magic_flags_test, closed_magic_set_flags_and_get_flags)
{
    magic m;
    EXPECT_THROW(m.set_flags(magic::flags::Mime), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_flags(), magic_is_closed);
}

TEST(magic_flags_test, opened_magic_set_flags_and_get_flags)
{
    magic m;
    m.open(magic::flags::None);
    std::mt19937 eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist{0, magic::flags_mask_t{}.size() - 1};
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
    m.set_flags(
        std::ranges::fold_left(test_flags.begin(), test_flags.end(), test_flags.front(), std::bit_or<decltype(1ULL)>{})
    );
    magic::flags_container_t expected_magic_flags;
    std::ranges::transform(test_flags, std::back_inserter(expected_magic_flags),
        [](unsigned long long value){return static_cast<magic::flags>(value);}
    );
    expected_magic_flags.erase(
        std::unique(expected_magic_flags.begin(), expected_magic_flags.end()), expected_magic_flags.end()
    );
    EXPECT_EQ(expected_magic_flags, m.get_flags());
}

TEST(magic_flags_test, magic_flags_to_string_conversion)
{
    EXPECT_EQ(
    to_string(magic::flags_container_t{
        magic::flags::None            ,
        magic::flags::Debug           ,
        magic::flags::Symlink         ,
        magic::flags::Compress        ,
        magic::flags::Devices         ,
        magic::flags::MimeType        ,
        magic::flags::Continue        ,
        magic::flags::Check           ,
        magic::flags::PreserveAtime   ,
        magic::flags::Raw             ,
        magic::flags::Error           ,
        magic::flags::MimeEncoding    ,
        magic::flags::Mime            ,
        magic::flags::Apple           ,
        magic::flags::Extension       ,
        magic::flags::CompressTransp  ,
        magic::flags::NoCompressFork  ,
        magic::flags::Nodesc          ,
        magic::flags::NoCheckCompress ,
        magic::flags::NoCheckTar      ,
        magic::flags::NoCheckSoft     ,
        magic::flags::NoCheckApptype  ,
        magic::flags::NoCheckElf      ,
        magic::flags::NoCheckText     ,
        magic::flags::NoCheckCdf      ,
        magic::flags::NoCheckCsv      ,
        magic::flags::NoCheckTokens   ,
        magic::flags::NoCheckEncoding ,
        magic::flags::NoCheckJson     ,
        magic::flags::NoCheckSimh     ,
        magic::flags::NoCheckBuiltin
    }),
        "None, "
        "Debug, "
        "Symlink, "
        "Compress, "
        "Devices, "
        "MimeType, "
        "Continue, "
        "Check, "
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
