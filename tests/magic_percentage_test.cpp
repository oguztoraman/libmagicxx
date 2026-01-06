/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

/**
 * @file magic_percentage_test.cpp
 * @brief Unit tests for Utility::Percentage class.
 *
 * Tests the Percentage value type that represents clamped
 * percentage values in the range [0, 100].
 *
 * @section percentage_test_cases Test Cases
 *
 * | Test Case | Description |
 * |-----------|-------------|
 * | default_constructor | Default constructs to 0% |
 * | value_constructor_in_range | Values 0-100 are preserved |
 * | value_constructor_out_of_range | Values are clamped to [0, 100] |
 * | set_in_range | Set() with valid values |
 * | set_out_of_range | Set() clamps out-of-range values |
 * | steps_constructor_normal | Construction from completed/total |
 * | steps_constructor_zero_total | Division by zero is handled |
 * | to_string | ToString() returns "N%" format |
 *
 * @see Utility::Percentage
 */

#include <gtest/gtest.h>

#include "percentage.hpp"

using Recognition::Utility::Percentage;

TEST(MagicPercentageTest, default_constructor)
{
    EXPECT_EQ(Percentage{}.Get(), 0);
}

TEST(MagicPercentageTest, value_constructor_in_range)
{
    EXPECT_EQ(Percentage{42}.Get(), 42);
    EXPECT_EQ(Percentage{100}.Get(), 100);
    EXPECT_EQ(Percentage{0}.Get(), 0);
}

TEST(MagicPercentageTest, value_constructor_out_of_range)
{
    EXPECT_EQ(Percentage{-10}.Get(), 0);
    EXPECT_EQ(Percentage{150}.Get(), 100);
}

TEST(MagicPercentageTest, set_in_range)
{
    Percentage test_percentage;
    test_percentage.Set(55);
    EXPECT_EQ(test_percentage.Get(), 55);
}

TEST(MagicPercentageTest, set_out_of_range)
{
    Percentage test_percentage;
    test_percentage.Set(-5);
    EXPECT_EQ(test_percentage.Get(), 0);
    test_percentage.Set(200);
    EXPECT_EQ(test_percentage.Get(), 100);
}

TEST(MagicPercentageTest, steps_constructor_normal)
{
    EXPECT_EQ((Percentage{25, 100}.Get()), 25);
    EXPECT_EQ((Percentage{50, 200}.Get()), 25);
    EXPECT_EQ((Percentage{100, 100}.Get()), 100);
}

TEST(MagicPercentageTest, steps_constructor_zero_total)
{
    Percentage{10, 0};
    SUCCEED();
}

TEST(MagicPercentageTest, to_string)
{
    EXPECT_EQ(Percentage{75}.ToString(), "75%");
}
