/* SPDX-FileCopyrightText: Copyright (c) 2024-2025 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <gtest/gtest.h>

#include "magic.hpp"

using namespace recognition;

TEST(magic_open_close_test, closed_magic_is_open)
{
    magic m;
    EXPECT_FALSE(m);
    EXPECT_FALSE(m.is_open());
}

TEST(magic_open_close_test, closed_magic_close)
{
    magic m;
    m.close();
    EXPECT_FALSE(m);
    EXPECT_FALSE(m.is_open());
}

TEST(magic_open_close_test, opened_magic_is_open)
{
    magic m;
    m.open(magic::flags::mime);
    EXPECT_TRUE(m);
    EXPECT_TRUE(m.is_open());
}

TEST(magic_open_close_test, opened_magic_close)
{
    magic m;
    m.open(magic::flags::mime);
    m.close();
    EXPECT_FALSE(m);
    EXPECT_FALSE(m.is_open());
}

TEST(magic_open_close_test, opened_magic_open_twice)
{
    magic m;
    m.open(magic::flags::mime);
    m.open(magic::flags::mime_type);
    EXPECT_TRUE(m);
    EXPECT_TRUE(m.is_open());
}

TEST(magic_open_close_test, opened_magic_close_twice)
{
    magic m;
    m.open(magic::flags::mime);
    m.close();
    m.close();
    EXPECT_FALSE(m);
    EXPECT_FALSE(m.is_open());
}

TEST(magic_open_close_test, open_magic_using_flags_container)
{
    magic m;
    m.open(magic::flags_container_t{
        magic::flags::mime,
        magic::flags::continue_search
    });
    EXPECT_TRUE(m);
    EXPECT_TRUE(m.is_open());
}
