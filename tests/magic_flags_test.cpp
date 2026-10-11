/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_flags_test.cpp
 * @brief Unit tests for MagicFlags and Magic flag operations.
 *
 * Tests the MagicFlags value class and the flag getting and setting
 * functionality of Magic, including:
 * - Every MagicFlags constructor (default, single flag, container, copy, move)
 * - Assignment operators and the full operator| overload family
 * - Grouped and chained operator| expressions
 * - Equality comparison via operator==
 * - Total ordering via operator<=> (mask order, ordered containers)
 * - Container constructor edge cases (None mixed in, large duplicated sets)
 * - Raw bit-position mapping of every flag
 * - ToContainer() ordering, membership, and round-trips
 * - ToString() name lookup for all flags and separator formatting
 * - Every Magic flags constructor form (mask, container, noexcept variants)
 * - Open() with flag sets and containers, throwing and noexcept variants
 * - SetFlags() with flag sets and containers on Magic instances
 * - GetFlags() retrieval, throwing and noexcept variants
 * - Behavior on closed, opened, and valid Magic instances
 * - Rejection of flag masks carrying bit positions that no `FlagsT`
 *   enumerator covers
 *
 * @section flags_test_strategy Test Strategy
 *
 * The MagicFlagsValueTest fixture covers the standalone class directly;
 * the MagicFlagsTest fixture uses randomly generated flag combinations to
 * ensure robust coverage of the flag space. Each integration test verifies:
 * - Correct exception throwing for closed Magic
 * - Proper return values for noexcept variants
 * - Round-trip consistency (set then get), which also proves the internal
 *   libmagic conversion matches the OR of the expected MAGIC_* constants
 *
 * @see MagicFlags
 * @see Magic::FlagsT
 * @see Magic::FlagsMaskT
 * @see Magic::SetFlags()
 * @see Magic::GetFlags()
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <bit>
#include <compare>
#include <cstddef>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "magic.hpp"

using namespace Recognition;

struct MagicFlagsTest : testing::Test {
protected:
    MagicFlagsTest()
    {
        EXPECT_TRUE(m_opened_magic_without_database.Open(
            Magic::FlagsT::Mime,
            std::nothrow
        ));
        EXPECT_TRUE(m_valid_magic.IsValid());
        std::error_code error_code;
        EXPECT_TRUE(std::filesystem::exists(m_valid_database, error_code));
    }

    void SetUp() override
    {
        std::vector<Magic::FlagsT> test_flags{
            static_cast<Magic::FlagsT>(1U << m_dist(m_eng)),
            static_cast<Magic::FlagsT>(1U << m_dist(m_eng)),
            static_cast<Magic::FlagsT>(1U << m_dist(m_eng)),
            static_cast<Magic::FlagsT>(1U << m_dist(m_eng)),
            static_cast<Magic::FlagsT>(1U << m_dist(m_eng)),
            static_cast<Magic::FlagsT>(1U << m_dist(m_eng)),
            static_cast<Magic::FlagsT>(1U << m_dist(m_eng))
        };
        std::ranges::sort(test_flags, [](Magic::FlagsT a, Magic::FlagsT b) {
            return std::to_underlying(a) < std::to_underlying(b);
        });
        m_test_flags_container.clear();
        m_test_flags_container.assign(test_flags.begin(), test_flags.end());
        m_test_flags_container.erase(
            std::unique(
                m_test_flags_container.begin(),
                m_test_flags_container.end()
            ),
            m_test_flags_container.end()
        );
        m_test_flags_mask = std::ranges::fold_left(
            m_test_flags_container,
            Magic::FlagsMaskT{},
            [](Magic::FlagsMaskT acc, Magic::FlagsT f) {
                return acc | f;
            }
        );
    }

    static constexpr std::size_t FLAG_BIT_COUNT{
        std::bit_width(std::to_underlying(Magic::FlagsT::NoCheckBuiltin))
    };

    static constexpr Magic::FlagsT FlagAtBit(std::size_t bit)
    {
        return static_cast<Magic::FlagsT>(1U << bit);
    }

    static constexpr std::size_t UNKNOWN_FLAG_BIT{31};

    static Magic::FlagsMaskT UnknownBitFlags()
    {
        return Magic::FlagsMaskT{
            static_cast<Magic::FlagsT>(1U << UNKNOWN_FLAG_BIT)
        };
    }

    static Magic::FlagsContainerT UnknownBitContainer()
    {
        return Magic::FlagsContainerT{
            Magic::FlagsT::Mime,
            static_cast<Magic::FlagsT>(1U << UNKNOWN_FLAG_BIT)
        };
    }

    static Magic::FlagsMaskT FirstUnknownBitFlags()
    {
        return Magic::FlagsMaskT{FlagAtBit(FLAG_BIT_COUNT)};
    }

    std::filesystem::path m_valid_database{MAGIC_DEFAULT_DATABASE_FILE};
    Magic                 m_closed_magic{};
    Magic                 m_opened_magic_without_database;
    Magic m_valid_magic{Magic::FlagsT::Mime, std::nothrow, m_valid_database};
    Magic::FlagsContainerT                     m_test_flags_container{};
    Magic::FlagsMaskT                          m_test_flags_mask{};
    std::mt19937                               m_eng{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> m_dist{0, FLAG_BIT_COUNT - 1};
};

TEST_F(MagicFlagsTest, closed_magic_set_flags_mask)
{
    EXPECT_THROW(m_closed_magic.SetFlags(m_test_flags_mask), MagicIsClosed);
}

TEST_F(MagicFlagsTest, closed_magic_set_flags_mask_noexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_mask, std::nothrow));
}

TEST_F(MagicFlagsTest, closed_magic_set_flags_container)
{
    EXPECT_THROW(
        m_closed_magic.SetFlags(m_test_flags_container),
        MagicIsClosed
    );
}

TEST_F(MagicFlagsTest, closed_magic_set_flags_container_noexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_container, std::nothrow));
}

TEST_F(MagicFlagsTest, closed_magic_get_flags)
{
    EXPECT_THROW(static_cast<void>(m_closed_magic.GetFlags()), MagicIsClosed);
}

TEST_F(MagicFlagsTest, closed_magic_get_flags_noexcept)
{
    EXPECT_FALSE(m_closed_magic.SetFlags(m_test_flags_mask, std::nothrow));
}

TEST_F(MagicFlagsTest, opened_magic_without_database_flags_mask)
{
    EXPECT_NO_THROW(
        m_opened_magic_without_database.SetFlags(m_test_flags_mask)
    );
    EXPECT_EQ(m_test_flags_mask, m_opened_magic_without_database.GetFlags());
}

TEST_F(MagicFlagsTest, opened_magic_without_database_flags_mask_noexcept)
{
    EXPECT_TRUE(m_opened_magic_without_database.SetFlags(
        m_test_flags_mask,
        std::nothrow
    ));
    EXPECT_EQ(
        m_test_flags_mask,
        m_opened_magic_without_database.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, opened_magic_without_database_flags_container)
{
    EXPECT_NO_THROW(
        m_opened_magic_without_database.SetFlags(m_test_flags_container)
    );
    EXPECT_EQ(m_test_flags_mask, m_opened_magic_without_database.GetFlags());
}

TEST_F(MagicFlagsTest, opened_magic_without_database_flags_container_noexcept)
{
    EXPECT_TRUE(m_opened_magic_without_database.SetFlags(
        m_test_flags_container,
        std::nothrow
    ));
    EXPECT_EQ(
        m_test_flags_mask,
        m_opened_magic_without_database.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, valid_magic_flags_mask)
{
    EXPECT_NO_THROW(m_valid_magic.SetFlags(m_test_flags_mask));
    EXPECT_EQ(m_test_flags_mask, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, valid_magic_flags_mask_noexcept)
{
    EXPECT_TRUE(m_valid_magic.SetFlags(m_test_flags_mask, std::nothrow));
    EXPECT_EQ(m_test_flags_mask, m_valid_magic.GetFlags(std::nothrow).value());
}

TEST_F(MagicFlagsTest, valid_magic_flags_container)
{
    EXPECT_NO_THROW(m_valid_magic.SetFlags(m_test_flags_container));
    EXPECT_EQ(m_test_flags_mask, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, valid_magic_flags_container_noexcept)
{
    EXPECT_TRUE(m_valid_magic.SetFlags(m_test_flags_container, std::nothrow));
    EXPECT_EQ(m_test_flags_mask, m_valid_magic.GetFlags(std::nothrow).value());
}

TEST_F(MagicFlagsTest, flags_mask_in_open_call)
{
    EXPECT_NO_THROW(m_closed_magic.Open(m_test_flags_mask));
    EXPECT_TRUE(m_closed_magic.IsOpen());
    EXPECT_EQ(m_test_flags_mask, m_closed_magic.GetFlags());
}

TEST_F(MagicFlagsTest, flags_container_in_open_call)
{
    EXPECT_NO_THROW(m_closed_magic.Open(m_test_flags_container));
    EXPECT_TRUE(m_closed_magic.IsOpen());
    EXPECT_EQ(m_test_flags_mask, m_closed_magic.GetFlags());
}

TEST_F(MagicFlagsTest, flags_mask_in_open_call_noexcept)
{
    EXPECT_TRUE(m_closed_magic.Open(m_test_flags_mask, std::nothrow));
    EXPECT_TRUE(m_closed_magic.IsOpen());
    EXPECT_EQ(m_test_flags_mask, m_closed_magic.GetFlags(std::nothrow).value());
}

TEST_F(MagicFlagsTest, flags_container_in_open_call_noexcept)
{
    EXPECT_TRUE(m_closed_magic.Open(m_test_flags_container, std::nothrow));
    EXPECT_TRUE(m_closed_magic.IsOpen());
    EXPECT_EQ(m_test_flags_mask, m_closed_magic.GetFlags(std::nothrow).value());
}

TEST_F(MagicFlagsTest, implicit_conversion_from_single_flag_in_open_call)
{
    Magic magic{};
    EXPECT_TRUE(magic.Open(Magic::FlagsT::Mime, std::nothrow));
    EXPECT_TRUE(magic.IsOpen());
    EXPECT_EQ(
        Magic::FlagsMaskT{Magic::FlagsT::Mime},
        magic.GetFlags(std::nothrow).value()
    );
}

TEST_F(MagicFlagsTest, combined_flags_in_open_call)
{
    Magic magic{};
    EXPECT_TRUE(magic.Open(
        Magic::FlagsT::MimeType | Magic::FlagsT::MimeEncoding,
        std::nothrow
    ));
    EXPECT_TRUE(magic.IsOpen());
}

TEST_F(MagicFlagsTest, flags_mask_in_constructor)
{
    const Magic magic{m_test_flags_mask, m_valid_database};
    EXPECT_TRUE(magic.IsValid());
    EXPECT_EQ(m_test_flags_mask, magic.GetFlags());
}

TEST_F(MagicFlagsTest, flags_mask_in_constructor_noexcept)
{
    const Magic magic{m_test_flags_mask, std::nothrow, m_valid_database};
    EXPECT_TRUE(magic.IsValid());
    EXPECT_EQ(m_test_flags_mask, magic.GetFlags());
}

TEST_F(MagicFlagsTest, flags_container_in_constructor)
{
    const Magic magic{m_test_flags_container, m_valid_database};
    EXPECT_TRUE(magic.IsValid());
    EXPECT_EQ(m_test_flags_mask, magic.GetFlags());
}

TEST_F(MagicFlagsTest, flags_container_in_constructor_noexcept)
{
    const Magic magic{m_test_flags_container, std::nothrow, m_valid_database};
    EXPECT_TRUE(magic.IsValid());
    EXPECT_EQ(m_test_flags_mask, magic.GetFlags());
}

TEST_F(MagicFlagsTest, braced_flag_list_in_constructor)
{
    const Magic magic{
        {Magic::FlagsT::Mime, Magic::FlagsT::Compress},
        std::nothrow,
        m_valid_database
    };
    EXPECT_TRUE(magic.IsValid());
    const Magic::FlagsMaskT expected{
        Magic::FlagsT::Compress | Magic::FlagsT::Mime
    };
    EXPECT_EQ(expected, magic.GetFlags());
}

TEST_F(MagicFlagsTest, parenthesized_flags_in_constructor)
{
    const Magic magic{
        Magic::FlagsT::Debug
            | (Magic::FlagsT::MimeType | Magic::FlagsT::MimeEncoding),
        std::nothrow,
        m_valid_database
    };
    EXPECT_TRUE(magic.IsValid());
}

TEST_F(MagicFlagsTest, set_flags_with_unknown_mask_bit_throws_and_keeps_flags)
{
    const Magic::FlagsMaskT before{m_valid_magic.GetFlags()};
    EXPECT_THROW(m_valid_magic.SetFlags(UnknownBitFlags()), MagicSetFlagsError);
    EXPECT_EQ(before, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, set_flags_with_unknown_mask_bit_noexcept_returns_false)
{
    const Magic::FlagsMaskT before{m_valid_magic.GetFlags()};
    EXPECT_FALSE(m_valid_magic.SetFlags(UnknownBitFlags(), std::nothrow));
    EXPECT_EQ(before, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, set_flags_with_unknown_container_bit_throws)
{
    const Magic::FlagsMaskT before{m_valid_magic.GetFlags()};
    EXPECT_THROW(
        m_valid_magic.SetFlags(UnknownBitContainer()),
        MagicSetFlagsError
    );
    EXPECT_EQ(before, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, open_with_unknown_mask_bit_throws)
{
    Magic magic{};
    EXPECT_THROW(magic.Open(UnknownBitFlags()), MagicOpenError);
    EXPECT_FALSE(magic.IsOpen());
}

TEST_F(MagicFlagsTest, open_with_unknown_mask_bit_noexcept_returns_false)
{
    Magic magic{};
    EXPECT_FALSE(magic.Open(UnknownBitFlags(), std::nothrow));
    EXPECT_FALSE(magic.IsOpen());
}

TEST_F(MagicFlagsTest, set_flags_with_first_unknown_bit_throws_and_keeps_flags)
{
    const Magic::FlagsMaskT before{m_valid_magic.GetFlags()};
    EXPECT_THROW(
        m_valid_magic.SetFlags(FirstUnknownBitFlags()),
        MagicSetFlagsError
    );
    EXPECT_EQ(before, m_valid_magic.GetFlags());
}

TEST_F(
    MagicFlagsTest,
    set_flags_with_unknown_container_bit_noexcept_returns_false
)
{
    const Magic::FlagsMaskT before{m_valid_magic.GetFlags()};
    EXPECT_FALSE(m_valid_magic.SetFlags(UnknownBitContainer(), std::nothrow));
    EXPECT_EQ(before, m_valid_magic.GetFlags());
}

TEST_F(MagicFlagsTest, open_with_unknown_container_bit_throws)
{
    Magic magic{};
    EXPECT_THROW(magic.Open(UnknownBitContainer()), MagicOpenError);
    EXPECT_FALSE(magic.IsOpen());
}

TEST_F(MagicFlagsTest, open_with_unknown_container_bit_noexcept_returns_false)
{
    Magic magic{};
    EXPECT_FALSE(magic.Open(UnknownBitContainer(), std::nothrow));
    EXPECT_FALSE(magic.IsOpen());
}

TEST_F(MagicFlagsTest, constructor_with_unknown_mask_bit_throws)
{
    EXPECT_THROW(
        static_cast<void>(Magic{UnknownBitFlags(), m_valid_database}),
        MagicOpenError
    );
}

TEST_F(MagicFlagsTest, constructor_with_unknown_mask_bit_noexcept_stays_closed)
{
    const Magic magic{UnknownBitFlags(), std::nothrow, m_valid_database};
    EXPECT_FALSE(magic.IsOpen());
    EXPECT_FALSE(magic.IsValid());
}

TEST_F(MagicFlagsTest, every_valid_flag_bit_still_maps_to_libmagic)
{
    for (std::size_t bit{}; bit < FLAG_BIT_COUNT; ++bit) {
        const Magic::FlagsT flag{FlagAtBit(bit)};
        EXPECT_TRUE(m_valid_magic.SetFlags(flag, std::nothrow)) << bit;
        const Magic::FlagsMaskT expected{flag};
        EXPECT_EQ(expected, m_valid_magic.GetFlags()) << bit;
        EXPECT_TRUE(m_opened_magic_without_database.Open(
            Magic::FlagsMaskT{flag},
            std::nothrow
        )) << bit;
        EXPECT_EQ(expected, m_opened_magic_without_database.GetFlags()) << bit;
    }
}

struct MagicFlagsValueTest : testing::Test {
    static const inline std::vector<std::pair<MagicFlags::Flags, std::string>>
        ALL_FLAGS{
            {MagicFlags::Flags::None,            "None"           },
            {MagicFlags::Flags::Debug,           "Debug"          },
            {MagicFlags::Flags::Symlink,         "Symlink"        },
            {MagicFlags::Flags::Compress,        "Compress"       },
            {MagicFlags::Flags::Devices,         "Devices"        },
            {MagicFlags::Flags::MimeType,        "MimeType"       },
            {MagicFlags::Flags::ContinueSearch,  "ContinueSearch" },
            {MagicFlags::Flags::CheckDatabase,   "CheckDatabase"  },
            {MagicFlags::Flags::PreserveAtime,   "PreserveAtime"  },
            {MagicFlags::Flags::Raw,             "Raw"            },
            {MagicFlags::Flags::Error,           "Error"          },
            {MagicFlags::Flags::MimeEncoding,    "MimeEncoding"   },
            {MagicFlags::Flags::Mime,            "Mime"           },
            {MagicFlags::Flags::Apple,           "Apple"          },
            {MagicFlags::Flags::Extension,       "Extension"      },
            {MagicFlags::Flags::CompressTransp,  "CompressTransp" },
            {MagicFlags::Flags::NoCompressFork,  "NoCompressFork" },
            {MagicFlags::Flags::Nodesc,          "Nodesc"         },
            {MagicFlags::Flags::NoCheckCompress, "NoCheckCompress"},
            {MagicFlags::Flags::NoCheckTar,      "NoCheckTar"     },
            {MagicFlags::Flags::NoCheckSoft,     "NoCheckSoft"    },
            {MagicFlags::Flags::NoCheckApptype,  "NoCheckApptype" },
            {MagicFlags::Flags::NoCheckElf,      "NoCheckElf"     },
            {MagicFlags::Flags::NoCheckText,     "NoCheckText"    },
            {MagicFlags::Flags::NoCheckCdf,      "NoCheckCdf"     },
            {MagicFlags::Flags::NoCheckCsv,      "NoCheckCsv"     },
            {MagicFlags::Flags::NoCheckTokens,   "NoCheckTokens"  },
            {MagicFlags::Flags::NoCheckEncoding, "NoCheckEncoding"},
            {MagicFlags::Flags::NoCheckJson,     "NoCheckJson"    },
            {MagicFlags::Flags::NoCheckSimh,     "NoCheckSimh"    },
            {MagicFlags::Flags::NoCheckBuiltin,  "NoCheckBuiltin" }
    };

    static std::size_t FlagBitCount()
    {
        return ALL_FLAGS.size() - 1UZ;
    }

    static constexpr MagicFlags::Flags FlagAtBit(std::size_t bit)
    {
        return static_cast<MagicFlags::Flags>(1U << bit);
    }

    static bool Contains(
        const MagicFlags::FlagsContainerT& container,
        MagicFlags::Flags                  flag
    )
    {
        return std::find(container.begin(), container.end(), flag)
            != container.end();
    }

    static MagicFlags AllFlagsSet()
    {
        MagicFlags flags;
        for (std::size_t i{1UZ}; i < ALL_FLAGS.size(); ++i) {
            flags = flags | ALL_FLAGS.at(i).first;
        }
        return flags;
    }
};

TEST_F(MagicFlagsValueTest, flag_bit_count_matches_enum)
{
    EXPECT_EQ(30UZ, FlagBitCount());
    EXPECT_EQ(
        std::bit_width(std::to_underlying(MagicFlags::Flags::NoCheckBuiltin)),
        FlagBitCount()
    );
}

TEST(MagicFlagsClassTest, satisfies_value_class_properties)
{
    static_assert(std::is_copy_constructible_v<MagicFlags>);
    static_assert(std::is_copy_assignable_v<MagicFlags>);
    static_assert(std::is_move_constructible_v<MagicFlags>);
    static_assert(std::is_move_assignable_v<MagicFlags>);
    static_assert(std::is_destructible_v<MagicFlags>);
    static_assert(std::is_nothrow_default_constructible_v<MagicFlags>);
    static_assert(std::is_nothrow_copy_constructible_v<MagicFlags>);
    static_assert(std::is_nothrow_move_constructible_v<MagicFlags>);
    static_assert(std::is_nothrow_copy_assignable_v<MagicFlags>);
    static_assert(std::is_nothrow_move_assignable_v<MagicFlags>);
    static_assert(std::is_nothrow_destructible_v<MagicFlags>);
    {
        const MagicFlags::FlagsContainerT container{MagicFlags::Flags::Mime};
        static_assert(
            std::bool_constant<noexcept(
                MagicFlags{MagicFlags::Flags::Mime}
            )>::value
            && std::bool_constant<noexcept(MagicFlags{container})>::value
        );
        (void)container;
    }
    {
        const MagicFlags lhs_mask{MagicFlags::Flags::Mime};
        const MagicFlags rhs_mask{MagicFlags::Flags::Compress};
        const auto       lhs_flag = MagicFlags::Flags::Debug;
        const auto       rhs_flag = MagicFlags::Flags::Symlink;
        static_assert(
            std::bool_constant<noexcept(lhs_mask | rhs_mask)>::value
            && std::bool_constant<noexcept(lhs_flag | rhs_flag)>::value
            && std::bool_constant<noexcept(lhs_mask | rhs_flag)>::value
            && std::bool_constant<noexcept(lhs_flag | rhs_mask)>::value
        );
        (void)lhs_mask;
        (void)rhs_mask;
        (void)lhs_flag;
        (void)rhs_flag;
    }
}

TEST_F(MagicFlagsValueTest, every_single_flag_sets_only_own_bit)
{
    for (std::size_t bit{}; bit < FlagBitCount(); ++bit) {
        const auto flag = FlagAtBit(bit);
        EXPECT_TRUE(Contains(MagicFlags{flag}.ToContainer(), flag));
        for (std::size_t other{}; other < FlagBitCount(); ++other) {
            if (other != bit) {
                EXPECT_FALSE(
                    Contains(MagicFlags{flag}.ToContainer(), FlagAtBit(other))
                );
            }
        }
    }
}

TEST_F(MagicFlagsValueTest, default_constructor_is_empty)
{
    const MagicFlags flags;
    EXPECT_EQ(
        MagicFlags::FlagsContainerT{MagicFlags::Flags::None},
        flags.ToContainer()
    );
    EXPECT_EQ("None", flags.ToString());
}

TEST_F(MagicFlagsValueTest, implicit_construction_from_every_flag)
{
    for (const auto& [flag, name] : ALL_FLAGS) {
        const MagicFlags flags = flag;
        EXPECT_EQ(MagicFlags::FlagsContainerT{flag}, flags.ToContainer());
        EXPECT_EQ(name, flags.ToString());
    }
}

TEST_F(MagicFlagsValueTest, to_string_name_lookup_for_every_flag)
{
    for (const auto& [flag, name] : ALL_FLAGS) {
        const MagicFlags flags{flag};
        EXPECT_EQ(name, flags.ToString());
    }
    const MagicFlags flags{
        MagicFlags::Flags::Debug | MagicFlags::Flags::Compress
    };
    EXPECT_EQ(flags.ToString(", "), flags.ToString());
}

TEST_F(MagicFlagsValueTest, every_bit_position_maps_to_expected_flag)
{
    for (std::size_t bit{}; bit < FlagBitCount(); ++bit) {
        const auto flag = ALL_FLAGS.at(bit + 1UZ).first;
        EXPECT_EQ(
            std::to_underlying(static_cast<MagicFlags::Flags>(1U << bit)),
            std::to_underlying(flag)
        );
    }
}

TEST_F(MagicFlagsValueTest, container_constructor_matches_pairwise_or)
{
    const MagicFlags::FlagsContainerT container{
        MagicFlags::Flags::Debug,
        MagicFlags::Flags::Compress,
        MagicFlags::Flags::Extension
    };
    const MagicFlags from_container{container};
    const MagicFlags pairwise = MagicFlags::Flags::Debug
                              | MagicFlags::Flags::Compress
                              | MagicFlags::Flags::Extension;
    EXPECT_EQ(from_container.ToContainer(), pairwise.ToContainer());
    EXPECT_EQ(from_container.ToString(), pairwise.ToString());
    EXPECT_EQ("Debug, Compress, Extension", pairwise.ToString());
}

TEST_F(MagicFlagsValueTest, empty_container_constructor_is_empty)
{
    const MagicFlags flags{MagicFlags::FlagsContainerT{}};
    EXPECT_EQ(
        MagicFlags::FlagsContainerT{MagicFlags::Flags::None},
        flags.ToContainer()
    );
    EXPECT_EQ("None", flags.ToString());
}

TEST_F(MagicFlagsValueTest, container_constructor_collapses_duplicates)
{
    const MagicFlags flags{
        MagicFlags::FlagsContainerT{
                                    MagicFlags::Flags::Mime,
                                    MagicFlags::Flags::Mime,
                                    MagicFlags::Flags::Mime
        }
    };
    EXPECT_EQ(1UZ, flags.ToContainer().size());
    EXPECT_EQ("Mime", flags.ToString());
}

TEST_F(
    MagicFlagsValueTest,
    container_constructor_with_none_mixed_in_keeps_other_flags
)
{
    const MagicFlags flags{
        MagicFlags::FlagsContainerT{
                                    MagicFlags::Flags::None,
                                    MagicFlags::Flags::Mime,
                                    MagicFlags::Flags::None
        }
    };
    EXPECT_EQ(MagicFlags{MagicFlags::Flags::Mime}, flags);
    EXPECT_EQ("Mime", flags.ToString());

    const MagicFlags only_none{
        MagicFlags::FlagsContainerT{
                                    MagicFlags::Flags::None,
                                    MagicFlags::Flags::None
        }
    };
    EXPECT_EQ(MagicFlags{}, only_none);
    EXPECT_EQ("None", only_none.ToString());
}

TEST_F(MagicFlagsValueTest, container_constructor_with_large_duplicated_set)
{
    MagicFlags::FlagsContainerT large_container{AllFlagsSet().ToContainer()};
    large_container.insert(
        large_container.end(),
        AllFlagsSet().ToContainer().begin(),
        AllFlagsSet().ToContainer().end()
    );
    large_container.push_back(MagicFlags::Flags::None);
    EXPECT_EQ(AllFlagsSet(), MagicFlags{large_container});
}

TEST_F(MagicFlagsValueTest, to_container_orders_by_ascending_bit)
{
    const MagicFlags flags{
        MagicFlags::Flags::Extension
        | (MagicFlags::Flags::Mime | MagicFlags::Flags::Debug)
    };
    const MagicFlags::FlagsContainerT expected{
        MagicFlags::Flags::Debug,
        MagicFlags::Flags::Mime,
        MagicFlags::Flags::Extension
    };
    EXPECT_EQ(expected, flags.ToContainer());
    const MagicFlags all_flags = AllFlagsSet();
    const auto       ordered   = all_flags.ToContainer();
    EXPECT_TRUE(
        std::adjacent_find(
            ordered.begin(),
            ordered.end(),
            [](MagicFlags::Flags a, MagicFlags::Flags b) {
                return std::to_underlying(a) >= std::to_underlying(b);
            }
        )
        == ordered.end()
    );
    EXPECT_EQ(ordered.size(), std::set(ordered.begin(), ordered.end()).size());
}

TEST_F(MagicFlagsValueTest, to_container_round_trips_through_constructor)
{
    const MagicFlags original{AllFlagsSet()};
    const MagicFlags rebuilt{original.ToContainer()};
    EXPECT_EQ(original.ToContainer(), rebuilt.ToContainer());
    EXPECT_EQ(original.ToString(), rebuilt.ToString());
}

TEST_F(MagicFlagsValueTest, copy_constructor_preserves_flags)
{
    const MagicFlags original{
        MagicFlags::Flags::Mime | MagicFlags::Flags::Compress
    };
    const MagicFlags copy{original};
    EXPECT_EQ(original.ToContainer(), copy.ToContainer());
    EXPECT_EQ("Compress, Mime", copy.ToString());
}

TEST_F(MagicFlagsValueTest, move_constructor_preserves_flags)
{
    MagicFlags original{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
    const MagicFlags moved{std::move(original)};
    EXPECT_EQ("Compress, Mime", moved.ToString());
}

TEST_F(MagicFlagsValueTest, copy_assignment_preserves_flags)
{
    const MagicFlags original{MagicFlags::Flags::Raw};
    MagicFlags       target{MagicFlags::Flags::Debug};
    target = original;
    EXPECT_EQ("Raw", target.ToString());
}

TEST_F(MagicFlagsValueTest, copy_self_assignment_preserves_flags)
{
    MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
    const auto expected = flags.ToContainer();
    auto&      alias    = flags;
    flags               = alias;
    EXPECT_EQ(expected, flags.ToContainer());
    EXPECT_EQ("Compress, Mime", flags.ToString());
}

TEST_F(MagicFlagsValueTest, move_self_assignment_leaves_usable_object)
{
    MagicFlags flags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress};
    auto&      alias = flags;
    flags            = std::move(alias);
    EXPECT_NO_THROW(static_cast<void>(flags.ToContainer()));
    EXPECT_NO_THROW(static_cast<void>(flags.ToString()));
}

TEST_F(MagicFlagsValueTest, move_assignment_preserves_flags)
{
    MagicFlags source{MagicFlags::Flags::Apple};
    MagicFlags target{MagicFlags::Flags::Debug};
    target = std::move(source);
    EXPECT_EQ("Apple", target.ToString());
}

TEST_F(MagicFlagsValueTest, operator_or_mask_with_mask)
{
    const MagicFlags mask_a{MagicFlags::Flags::Debug};
    const MagicFlags mask_b{MagicFlags::Flags::Compress};
    EXPECT_EQ("Debug, Compress", (mask_a | mask_b).ToString());
}

TEST_F(MagicFlagsValueTest, operator_or_flag_with_flag)
{
    EXPECT_EQ(
        "Debug, Symlink",
        (MagicFlags::Flags::Debug | MagicFlags::Flags::Symlink).ToString()
    );
}

TEST_F(MagicFlagsValueTest, operator_or_mask_with_flag)
{
    const MagicFlags mask{MagicFlags::Flags::Debug};
    EXPECT_EQ("Debug, Symlink", (mask | MagicFlags::Flags::Symlink).ToString());
}

TEST_F(MagicFlagsValueTest, operator_or_flag_with_mask)
{
    const MagicFlags mask{MagicFlags::Flags::Symlink};
    EXPECT_EQ("Debug, Symlink", (MagicFlags::Flags::Debug | mask).ToString());
}

TEST_F(MagicFlagsValueTest, operator_or_with_none_keeps_other_operand)
{
    const MagicFlags flags{MagicFlags::Flags::Mime};
    EXPECT_EQ("Mime", (flags | MagicFlags::Flags::None).ToString());
    EXPECT_EQ("Mime", (MagicFlags::Flags::None | flags).ToString());
    EXPECT_EQ("None", (MagicFlags{} | MagicFlags{}).ToString());
}

TEST_F(MagicFlagsValueTest, operator_or_is_idempotent)
{
    const MagicFlags flags{
        MagicFlags::Flags::Mime
        | MagicFlags::Flags::Mime
        | MagicFlags::Flags::Mime
    };
    EXPECT_EQ(1UZ, flags.ToContainer().size());
    EXPECT_EQ("Mime", flags.ToString());
}

TEST_F(MagicFlagsValueTest, composite_flags_are_single_bits)
{
    const MagicFlags mime{MagicFlags::Flags::Mime};
    EXPECT_EQ(
        MagicFlags::FlagsContainerT{MagicFlags::Flags::Mime},
        mime.ToContainer()
    );
    EXPECT_EQ("Mime", mime.ToString());

    const MagicFlags                  nodesc = MagicFlags::Flags::Debug
                                             | MagicFlags::Flags::Nodesc;
    const MagicFlags::FlagsContainerT expected{
        MagicFlags::Flags::Debug,
        MagicFlags::Flags::Nodesc
    };
    EXPECT_EQ(expected, nodesc.ToContainer());
    EXPECT_EQ("Debug, Nodesc", nodesc.ToString());
}

TEST_F(MagicFlagsValueTest, operator_or_grouped_expressions)
{
    const MagicFlags left_grouped = (MagicFlags::Flags::Debug
                                     | MagicFlags::Flags::Symlink)
                                  | MagicFlags::Flags::Compress;
    EXPECT_EQ("Debug, Symlink, Compress", left_grouped.ToString());

    const MagicFlags right_grouped = MagicFlags::Flags::Debug
                                   | (MagicFlags::Flags::Symlink
                                      | MagicFlags::Flags::Compress);
    EXPECT_EQ(left_grouped.ToContainer(), right_grouped.ToContainer());

    const MagicFlags both_grouped = (MagicFlags::Flags::Debug
                                     | MagicFlags::Flags::Symlink)
                                  | (MagicFlags::Flags::Compress
                                     | MagicFlags::Flags::Devices);
    const MagicFlags::FlagsContainerT expected{
        MagicFlags::Flags::Debug,
        MagicFlags::Flags::Symlink,
        MagicFlags::Flags::Compress,
        MagicFlags::Flags::Devices
    };
    EXPECT_EQ(expected, both_grouped.ToContainer());
}

TEST_F(MagicFlagsValueTest, operator_or_chained_expression)
{
    const MagicFlags flags = MagicFlags::Flags::Debug
                           | MagicFlags::Flags::Symlink
                           | MagicFlags::Flags::Compress
                           | MagicFlags::Flags::Devices
                           | MagicFlags::Flags::MimeType;
    EXPECT_EQ(5UZ, flags.ToContainer().size());
    EXPECT_EQ("Debug, Symlink, Compress, Devices, MimeType", flags.ToString());
}

TEST_F(MagicFlagsValueTest, equality_operator_compares_flag_sets)
{
    const MagicFlags empty_flags{};
    EXPECT_EQ(empty_flags, MagicFlags::Flags::None);
    EXPECT_EQ(empty_flags, MagicFlags{MagicFlags::FlagsContainerT{}});

    const MagicFlags from_container{
        MagicFlags::FlagsContainerT{
                                    MagicFlags::Flags::Debug,
                                    MagicFlags::Flags::Compress
        }
    };
    const MagicFlags from_or = MagicFlags::Flags::Debug
                             | MagicFlags::Flags::Compress;
    EXPECT_EQ(from_container, from_or);

    EXPECT_NE(
        MagicFlags{MagicFlags::Flags::Mime},
        MagicFlags{MagicFlags::Flags::Compress}
    );
    EXPECT_NE(
        MagicFlags{MagicFlags::Flags::Mime},
        MagicFlags{MagicFlags::Flags::Mime | MagicFlags::Flags::Compress}
    );
    EXPECT_NE(empty_flags, MagicFlags::Flags::Debug);

    const MagicFlags symmetry_lhs = MagicFlags::Flags::Apple
                                  | MagicFlags::Flags::Raw;
    const MagicFlags symmetry_rhs = MagicFlags::Flags::Raw
                                  | MagicFlags::Flags::Apple;
    EXPECT_EQ(symmetry_lhs, symmetry_rhs);
    EXPECT_NE(symmetry_lhs, AllFlagsSet());
    EXPECT_NE(AllFlagsSet(), symmetry_lhs);
}

TEST_F(MagicFlagsValueTest, spaceship_orders_by_underlying_mask)
{
    const MagicFlags empty_flags{};
    const MagicFlags debug{MagicFlags::Flags::Debug};
    const MagicFlags symlink{MagicFlags::Flags::Symlink};
    const MagicFlags mime{MagicFlags::Flags::Mime};

    EXPECT_LT(debug, symlink);
    EXPECT_LT(symlink, mime);
    EXPECT_LE(debug, debug);
    EXPECT_GE(debug, debug);
    EXPECT_GT(mime, debug);
    EXPECT_GT(mime, empty_flags);
    EXPECT_LT(empty_flags, debug);

    EXPECT_EQ(debug <=> symlink, std::strong_ordering::less);
    EXPECT_EQ(mime <=> debug, std::strong_ordering::greater);
    EXPECT_EQ(debug <=> debug, std::strong_ordering::equal);
    EXPECT_EQ(debug <=> MagicFlags::Flags::Debug, std::strong_ordering::equal);
}

TEST_F(MagicFlagsValueTest, spaceship_provides_strict_weak_ordering)
{
    const MagicFlags superset{
        MagicFlags::Flags::Debug | MagicFlags::Flags::Symlink
    };
    const MagicFlags high_bit{MagicFlags::Flags::NoCheckBuiltin};
    EXPECT_LT(superset, high_bit);

    MagicFlags::FlagsContainerT shuffled;
    shuffled.reserve(ALL_FLAGS.size() - 1UZ);
    for (std::size_t i{1UZ}; i < ALL_FLAGS.size(); ++i) {
        shuffled.push_back(ALL_FLAGS.at(i).first);
    }
    std::ranges::shuffle(shuffled, std::mt19937{42U});

    const std::set<MagicFlags> ordered_set{shuffled.begin(), shuffled.end()};
    EXPECT_EQ(ALL_FLAGS.size() - 1UZ, ordered_set.size());

    std::vector<MagicFlags> flags_vector{shuffled.begin(), shuffled.end()};
    std::ranges::sort(flags_vector);
    EXPECT_TRUE(std::ranges::is_sorted(flags_vector));
    EXPECT_TRUE(
        std::adjacent_find(flags_vector.begin(), flags_vector.end())
        == flags_vector.end()
    );

    std::size_t expected_bit{};
    for (const auto& flags : ordered_set) {
        EXPECT_EQ(flags, MagicFlags{FlagAtBit(expected_bit)});
        ++expected_bit;
    }
    EXPECT_EQ(expected_bit, ALL_FLAGS.size() - 1UZ);
}

TEST_F(MagicFlagsValueTest, to_string_joins_names_with_custom_separator)
{
    const MagicFlags flags{
        MagicFlags::Flags::Debug | MagicFlags::Flags::Compress
    };
    EXPECT_EQ("Debug, Compress", flags.ToString());
    EXPECT_EQ("Debug;Compress", flags.ToString(";"));
    EXPECT_EQ("Debug\nCompress", flags.ToString("\n"));
    EXPECT_EQ("Debug and Compress", flags.ToString(" and "));
}

TEST_F(MagicFlagsValueTest, to_string_empty_set_is_none_with_any_separator)
{
    EXPECT_EQ("None", MagicFlags{}.ToString());
    EXPECT_EQ("None", MagicFlags{}.ToString(";"));
    EXPECT_EQ("None", MagicFlags{MagicFlags::Flags::None}.ToString("\n"));
    EXPECT_EQ("None", MagicFlags{MagicFlags::FlagsContainerT{}}.ToString(";"));
}

TEST_F(MagicFlagsValueTest, to_string_of_every_flag_in_bit_order)
{
    const auto expected = std::accumulate(
        ALL_FLAGS.begin() + 1,
        ALL_FLAGS.end(),
        std::string{},
        [](std::string accumulator, const auto& entry) {
            if (!accumulator.empty()) {
                accumulator += ", ";
            }
            return accumulator + entry.second;
        }
    );
    EXPECT_EQ(expected, AllFlagsSet().ToString());
    EXPECT_EQ(expected, AllFlagsSet().ToString(", "));
}
