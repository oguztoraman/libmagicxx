/* SPDX-FileCopyrightText: Copyright (c) 2022-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace recognition;

TEST(magic_special_members_test, move_construct_magic_from_closed_magic)
{
    magic other;
    auto  m{std::move(other)};
    EXPECT_FALSE(m.is_open());
    EXPECT_FALSE(other.is_open());
    EXPECT_FALSE(m.is_valid());
    EXPECT_FALSE(other.is_valid());
}

TEST(magic_special_members_test, move_construct_magic_from_opened_magic)
{
    magic other;
    other.open(magic::flags::mime);
    auto m{std::move(other)};
    EXPECT_TRUE(m.is_open());
    EXPECT_FALSE(other.is_open());
    EXPECT_FALSE(m.is_valid());
    EXPECT_FALSE(other.is_valid());
}

TEST(magic_special_members_test, move_assign_magic_from_closed_magic)
{
    magic other;
    auto  m = std::move(other);
    EXPECT_FALSE(m.is_open());
    EXPECT_FALSE(other.is_open());
    EXPECT_FALSE(m.is_valid());
    EXPECT_FALSE(other.is_valid());
}

TEST(magic_special_members_test, move_assign_magic_from_opened_magic)
{
    magic other;
    other.open(magic::flags::mime);
    auto m = std::move(other);
    EXPECT_TRUE(m.is_open());
    EXPECT_FALSE(other.is_open());
    EXPECT_FALSE(m.is_valid());
    EXPECT_FALSE(other.is_valid());
}
