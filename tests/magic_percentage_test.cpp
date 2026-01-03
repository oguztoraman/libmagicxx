/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "percentage.hpp"

using Recognition::Utility::Percentage;

TEST(MagicPercentageTest, DefaultConstructor)
{
    EXPECT_EQ(Percentage{}.Get(), 0);
}

TEST(MagicPercentageTest, ValueConstructorInRange)
{
    EXPECT_EQ(Percentage{42}.Get(), 42);
    EXPECT_EQ(Percentage{100}.Get(), 100);
    EXPECT_EQ(Percentage{0}.Get(), 0);
}

TEST(MagicPercentageTest, ValueConstructorOutOfRange)
{
    EXPECT_EQ(Percentage{-10}.Get(), 0);
    EXPECT_EQ(Percentage{150}.Get(), 100);
}

TEST(MagicPercentageTest, SetInRange)
{
    Percentage test_percentage;
    test_percentage.Set(55);
    EXPECT_EQ(test_percentage.Get(), 55);
}

TEST(MagicPercentageTest, SetOutOfRange)
{
    Percentage test_percentage;
    test_percentage.Set(-5);
    EXPECT_EQ(test_percentage.Get(), 0);
    test_percentage.Set(200);
    EXPECT_EQ(test_percentage.Get(), 100);
}

TEST(MagicPercentageTest, StepsContructorNormal)
{
    EXPECT_EQ((Percentage{25, 100}.Get()), 25);
    EXPECT_EQ((Percentage{50, 200}.Get()), 25);
    EXPECT_EQ((Percentage{100, 100}.Get()), 100);
}

TEST(MagicPercentageTest, StepsContructorZeroTotal)
{
    Percentage{10, 0};
    SUCCEED();
}

TEST(MagicPercentageTest, ToStringTest)
{
    EXPECT_EQ(Percentage{75}.ToString(), "75%");
}
