/* SPDX-FileCopyrightText: Copyright (c) 2024 Oğuz Toraman <oguz.toraman@tutanota.com> */
/* SPDX-License-Identifier: LGPL-3.0-only */

#include <magic.hpp>
#include <gtest/gtest.h>

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
    m.open(magic::flags::Mime);
    EXPECT_TRUE(m);
    EXPECT_TRUE(m.is_open());
}

TEST(magic_open_close_test, opened_magic_close)
{
    magic m;
    m.open(magic::flags::Mime);
    m.close();
    EXPECT_FALSE(m);
    EXPECT_FALSE(m.is_open());
}

TEST(magic_open_close_test, opened_magic_open_twice)
{
    magic m;
    m.open(magic::flags::Mime);
    m.open(magic::flags::MimeType);
    EXPECT_TRUE(m);
    EXPECT_TRUE(m.is_open());
}

TEST(magic_open_close_test, opened_magic_close_twice)
{
    magic m;
    m.open(magic::flags::Mime);
    m.close();
    m.close();
    EXPECT_FALSE(m);
    EXPECT_FALSE(m.is_open());
}
