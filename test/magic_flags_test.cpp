/* SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <random>
#include <sstream>

#include <magic.hpp>
#include <gtest/gtest.h>

using namespace recognition;

TEST(magic_flags_test, closed_magic_set_flags_and_get_flags)
{
    magic m;
    EXPECT_THROW(m.set_flags(magic::Flag::Mime), magic_is_closed);
    EXPECT_THROW([[maybe_unused]] auto _ = m.get_flags(), magic_is_closed);
}

TEST(magic_flags_test, opened_magic_set_flags_and_get_flags)
{
    magic m;
    m.open(magic::Flag::None);
    std::mt19937 eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> dist{0, magic::flags_t{}.size() - 1};
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
    magic::Flags expected_magic_flags;
    std::ranges::transform(test_flags, std::back_inserter(expected_magic_flags),
        [](unsigned long long value){return static_cast<magic::Flag>(value);}
    );
    expected_magic_flags.erase(
        std::unique(expected_magic_flags.begin(), expected_magic_flags.end()), expected_magic_flags.end()
    );
    EXPECT_EQ(expected_magic_flags, m.get_flags());
}

TEST(magic_flags_test, magic_flags_to_string_conversion)
{
    EXPECT_EQ(
    to_string(magic::Flags{
        magic::Flag::None            ,
        magic::Flag::Debug           ,
        magic::Flag::Symlink         ,
        magic::Flag::Compress        ,
        magic::Flag::Devices         ,
        magic::Flag::MimeType        ,
        magic::Flag::Continue        ,
        magic::Flag::Check           ,
        magic::Flag::PreserveAtime   ,
        magic::Flag::Raw             ,
        magic::Flag::Error           ,
        magic::Flag::MimeEncoding    ,
        magic::Flag::Mime            ,
        magic::Flag::Apple           ,
        magic::Flag::Extension       ,
        magic::Flag::CompressTransp  ,
        magic::Flag::NoCompressFork  ,
        magic::Flag::Nodesc          ,
        magic::Flag::NoCheckCompress ,
        magic::Flag::NoCheckTar      ,
        magic::Flag::NoCheckSoft     ,
        magic::Flag::NoCheckApptype  ,
        magic::Flag::NoCheckElf      ,
        magic::Flag::NoCheckText     ,
        magic::Flag::NoCheckCdf      ,
        magic::Flag::NoCheckCsv      ,
        magic::Flag::NoCheckTokens   ,
        magic::Flag::NoCheckEncoding ,
        magic::Flag::NoCheckJson     ,
        magic::Flag::NoCheckSimh     ,
        magic::Flag::NoCheckBuiltin
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
