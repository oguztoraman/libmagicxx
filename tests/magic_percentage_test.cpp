/* SPDX-FileCopyrightText: Copyright (c) 2022-2026 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "percentage.hpp"

using recognition::utility::percentage;

TEST(magic_percentage_test, default_constructor)
{
    EXPECT_EQ(percentage{}.get(), 0);
}

TEST(magic_percentage_test, value_constructor_in_range)
{
    EXPECT_EQ(percentage{42}.get(), 42);
    EXPECT_EQ(percentage{100}.get(), 100);
    EXPECT_EQ(percentage{0}.get(), 0);
}

TEST(magic_percentage_test, value_constructor_out_of_range)
{
    EXPECT_EQ(percentage{-10}.get(), 0);
    EXPECT_EQ(percentage{150}.get(), 100);
}

TEST(magic_percentage_test, set_in_range)
{
    percentage test_percentage;
    test_percentage.set(55);
    EXPECT_EQ(test_percentage.get(), 55);
}

TEST(magic_percentage_test, set_out_of_range)
{
    percentage test_percentage;
    test_percentage.set(-5);
    EXPECT_EQ(test_percentage.get(), 0);
    test_percentage.set(200);
    EXPECT_EQ(test_percentage.get(), 100);
}

TEST(magic_percentage_test, steps_contructor_normal)
{
    EXPECT_EQ((percentage{25, 100}.get()), 25);
    EXPECT_EQ((percentage{50, 200}.get()), 25);
    EXPECT_EQ((percentage{100, 100}.get()), 100);
}

TEST(magic_percentage_test, steps_contructor_zero_total)
{
    percentage{10, 0};
    SUCCEED();
}

TEST(magic_percentage_test, to_string)
{
    EXPECT_EQ(percentage{75}.to_string(), "75%");
}
