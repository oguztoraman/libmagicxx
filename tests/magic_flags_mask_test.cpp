/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_flags_mask_test.cpp
 * @brief Unit tests for Magic::FlagsMaskT bridge class.
 *
 * Tests the FlagsMask class that bridges the scoped enum class Flags
 * with the internal bitmask representation, including:
 * - Default construction
 * - Implicit conversion from Flags
 * - Bitwise OR combinations (mask|mask, flag|flag, flag|mask, mask|flag)
 * - Bit access via operator[]
 * - size() and none() queries
 * - Parenthesized / grouped flag expressions
 *
 * @section flags_mask_test_strategy Test Strategy
 *
 * Tests verify both compile-time (constexpr) and runtime behavior
 * of the FlagsMask class to ensure type-safe flag combination works
 * correctly in all supported expression forms.
 *
 * @see Magic::FlagsMask
 * @see Magic::Flags
 */

#include <gtest/gtest.h>

#include <utility>

#include "magic.hpp"

using namespace Recognition;

struct MagicFlagsMaskTest : testing::Test { };

TEST_F(MagicFlagsMaskTest, default_constructor_is_empty)
{
    Magic::FlagsMaskT mask{};
    EXPECT_TRUE(mask.none());
}

TEST_F(MagicFlagsMaskTest, default_constructor_size)
{
    Magic::FlagsMaskT mask{};
    EXPECT_EQ(30uz, mask.size());
}

TEST_F(MagicFlagsMaskTest, implicit_conversion_from_single_flag)
{
    Magic::FlagsMaskT mask = Magic::Flags::Mime;
    EXPECT_FALSE(mask.none());
    EXPECT_TRUE(mask[11uz]);
}

TEST_F(MagicFlagsMaskTest, implicit_conversion_from_none_flag)
{
    Magic::FlagsMaskT mask = Magic::Flags::None;
    EXPECT_TRUE(mask.none());
}

TEST_F(MagicFlagsMaskTest, operator_subscript_correct_bit)
{
    Magic::FlagsMaskT mask = Magic::Flags::Debug;
    EXPECT_TRUE(mask[0uz]);
    for (std::size_t i{1uz}; i < mask.size(); ++i) {
        EXPECT_FALSE(mask[i]);
    }
}

TEST_F(MagicFlagsMaskTest, operator_subscript_symlink)
{
    Magic::FlagsMaskT mask = Magic::Flags::Symlink;
    EXPECT_TRUE(mask[1uz]);
    EXPECT_FALSE(mask[0uz]);
    EXPECT_FALSE(mask[2uz]);
}

TEST_F(MagicFlagsMaskTest, operator_or_two_flags)
{
    Magic::FlagsMaskT mask = Magic::Flags::MimeType | Magic::Flags::MimeEncoding;
    EXPECT_TRUE(mask[4uz]);
    EXPECT_TRUE(mask[10uz]);
    EXPECT_FALSE(mask[0uz]);
}

TEST_F(MagicFlagsMaskTest, operator_or_mask_with_mask)
{
    Magic::FlagsMaskT mask_a = Magic::Flags::Debug;
    Magic::FlagsMaskT mask_b = Magic::Flags::Compress;
    Magic::FlagsMaskT combined = mask_a | mask_b;
    EXPECT_TRUE(combined[0uz]);
    EXPECT_TRUE(combined[2uz]);
    EXPECT_FALSE(combined[1uz]);
}

TEST_F(MagicFlagsMaskTest, operator_or_mask_with_flag)
{
    Magic::FlagsMaskT mask = Magic::Flags::Debug;
    Magic::FlagsMaskT combined = mask | Magic::Flags::Symlink;
    EXPECT_TRUE(combined[0uz]);
    EXPECT_TRUE(combined[1uz]);
}

TEST_F(MagicFlagsMaskTest, operator_or_flag_with_mask)
{
    Magic::FlagsMaskT mask = Magic::Flags::Debug | Magic::Flags::Symlink;
    Magic::FlagsMaskT combined = Magic::Flags::Compress | mask;
    EXPECT_TRUE(combined[0uz]);
    EXPECT_TRUE(combined[1uz]);
    EXPECT_TRUE(combined[2uz]);
}

TEST_F(MagicFlagsMaskTest, parenthesized_right_group)
{
    Magic::FlagsMaskT mask =
        Magic::Flags::Debug | (Magic::Flags::Symlink | Magic::Flags::Compress);
    EXPECT_TRUE(mask[0uz]);
    EXPECT_TRUE(mask[1uz]);
    EXPECT_TRUE(mask[2uz]);
}

TEST_F(MagicFlagsMaskTest, parenthesized_left_group)
{
    Magic::FlagsMaskT mask =
        (Magic::Flags::Debug | Magic::Flags::Symlink) | Magic::Flags::Compress;
    EXPECT_TRUE(mask[0uz]);
    EXPECT_TRUE(mask[1uz]);
    EXPECT_TRUE(mask[2uz]);
}

TEST_F(MagicFlagsMaskTest, parenthesized_both_groups)
{
    Magic::FlagsMaskT mask = (Magic::Flags::Debug | Magic::Flags::Symlink)
                          | (Magic::Flags::Compress | Magic::Flags::Devices);
    EXPECT_TRUE(mask[0uz]);
    EXPECT_TRUE(mask[1uz]);
    EXPECT_TRUE(mask[2uz]);
    EXPECT_TRUE(mask[3uz]);
    EXPECT_FALSE(mask[4uz]);
}

TEST_F(MagicFlagsMaskTest, chained_or_multiple_flags)
{
    Magic::FlagsMaskT mask = Magic::Flags::Debug | Magic::Flags::Symlink
                          | Magic::Flags::Compress | Magic::Flags::Devices
                          | Magic::Flags::MimeType;
    EXPECT_TRUE(mask[0uz]);
    EXPECT_TRUE(mask[1uz]);
    EXPECT_TRUE(mask[2uz]);
    EXPECT_TRUE(mask[3uz]);
    EXPECT_TRUE(mask[4uz]);
}

TEST_F(MagicFlagsMaskTest, or_with_same_flag_is_idempotent)
{
    Magic::FlagsMaskT mask =
        Magic::Flags::Mime | Magic::Flags::Mime | Magic::Flags::Mime;
    EXPECT_TRUE(mask[11uz]);
    std::size_t set_count{};
    for (std::size_t i{}; i < mask.size(); ++i) {
        if (mask[i]) {
            ++set_count;
        }
    }
    EXPECT_EQ(1uz, set_count);
}

TEST_F(MagicFlagsMaskTest, none_returns_true_for_none_flag)
{
    Magic::FlagsMaskT mask = Magic::Flags::None;
    EXPECT_TRUE(mask.none());
}

TEST_F(MagicFlagsMaskTest, none_returns_false_for_set_flag)
{
    Magic::FlagsMaskT mask = Magic::Flags::Extension;
    EXPECT_FALSE(mask.none());
}

TEST_F(MagicFlagsMaskTest, size_is_always_thirty)
{
    Magic::FlagsMaskT empty{};
    Magic::FlagsMaskT single = Magic::Flags::Debug;
    Magic::FlagsMaskT combined = Magic::Flags::Debug | Magic::Flags::Symlink;
    EXPECT_EQ(30uz, empty.size());
    EXPECT_EQ(30uz, single.size());
    EXPECT_EQ(30uz, combined.size());
}

TEST_F(MagicFlagsMaskTest, implicit_conversion_in_open_call)
{
    Magic magic{};
    EXPECT_TRUE(magic.Open(Magic::Flags::Mime, std::nothrow));
    EXPECT_TRUE(magic.IsOpen());
}

TEST_F(MagicFlagsMaskTest, combined_flags_in_open_call)
{
    Magic magic{};
    EXPECT_TRUE(magic.Open(
        Magic::Flags::MimeType | Magic::Flags::MimeEncoding, std::nothrow
    ));
    EXPECT_TRUE(magic.IsOpen());
}

TEST_F(MagicFlagsMaskTest, parenthesized_flags_in_constructor)
{
    Magic magic{
        Magic::Flags::Debug
            | (Magic::Flags::MimeType | Magic::Flags::MimeEncoding),
        std::nothrow
    };
    EXPECT_TRUE(magic.IsOpen());
}

TEST_F(MagicFlagsMaskTest, all_individual_flags_set_correct_bit)
{
    static constexpr std::size_t FLAG_COUNT{30uz};
    for (std::size_t bit{}; bit < FLAG_COUNT; ++bit) {
        auto flag = static_cast<Magic::Flags>(1ULL << bit);
        Magic::FlagsMaskT mask = flag;
        EXPECT_TRUE(mask[bit]) << "bit " << bit << " should be set";
        for (std::size_t other{}; other < FLAG_COUNT; ++other) {
            if (other != bit) {
                EXPECT_FALSE(mask[other])
                    << "bit " << other << " should not be set when bit " << bit
                    << " is the only flag";
            }
        }
    }
}
